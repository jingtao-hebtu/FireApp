#include "ExperimentParamManager.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QMutexLocker>

#include "DbManager.h"
#include "TFMeaManager.h"

namespace {
    constexpr int kChannelDist = 1;
    constexpr int kChannelTilt = 2;
    constexpr int kChannelFireHeight = 3;
    constexpr int kChannelFireArea = 4;
}

namespace TF {

    ExperimentDbWorker::ExperimentDbWorker(QObject *parent) : QObject(parent) {
    }

    void ExperimentDbWorker::enqueue(const ExperimentRecord &record) {
        QMutexLocker locker(&mMutex);
        mQueue.enqueue(record);
        mCond.wakeOne();
    }

    void ExperimentDbWorker::startWork() {
        mRunning.store(true);

        while (mRunning.load() || !mQueue.isEmpty()) {
            ExperimentRecord record;
            {
                QMutexLocker locker(&mMutex);
                if (mQueue.isEmpty()) {
                    mCond.wait(&mMutex, 200);
                    if (mQueue.isEmpty()) {
                        continue;
                    }
                }

                record = mQueue.dequeue();
            }

            process(record);
        }

        QMutexLocker locker(&mMutex);
        mQueue.clear();
    }

    void ExperimentDbWorker::stopWork() {
        mRunning.store(false);
        mCond.wakeAll();
    }

    void ExperimentDbWorker::process(const ExperimentRecord &record) {
        DbManager::DataRow rows[] = {
            {record.expId, kChannelDist, record.sampleId, record.timestampMs, record.dist},
            {record.expId, kChannelTilt, record.sampleId, record.timestampMs, record.tilt},
            {record.expId, kChannelFireHeight, record.sampleId, record.timestampMs, record.fireHeight},
            {record.expId, kChannelFireArea, record.sampleId, record.timestampMs, record.fireArea},
        };

        try {
            DbManager::instance().InsertBatch(rows, DbManager::ConflictPolicy::Replace);
            if (!record.imagePath.isEmpty()) {
                DbManager::instance().UpsertDetectImage(record.expId, record.sampleId,
                                                        record.imagePath.toStdString());
            }
        }
        catch (...) {
            // DB 层已加锁保护，异常打印由上层处理
        }
    }

    ExperimentParamManager::ExperimentParamManager(QObject *parent) : QObject(parent) {
    }

    ExperimentParamManager::~ExperimentParamManager() {
        stopRecording();
        shutdownWorker();
    }

    bool ExperimentParamManager::experimentNameExists(const QString &name) const {
        return DbManager::instance().ExperimentNameExists(name.toStdString());
    }

    bool ExperimentParamManager::startRecording(const QString &name, QString *error) {
        const auto trimmed = name.trimmed();
        if (trimmed.isEmpty()) {
            if (error) *error = tr("实验名称不能为空");
            return false;
        }

        if (experimentNameExists(trimmed)) {
            if (error) *error = tr("该实验名称已存在，请重新输入");
            return false;
        }

        const auto now = currentTimestampMs();
        const int expId = DbManager::instance().CreateExperiment(trimmed.toStdString(), now);
        if (expId < 0) {
            if (error) *error = tr("创建实验失败");
            return false;
        }

        mExperimentName = trimmed;
        mExperimentId = expId;
        mNextSampleId = -1;
        mRecording.store(true);
        ensureWorker();
        if (mWorkerThread && !mWorkerThread->isRunning()) {
            mWorkerThread->start();
        }

        return true;
    }

    void ExperimentParamManager::stopRecording() {
        if (!mRecording.exchange(false)) {
            return;
        }

        const auto endTs = currentTimestampMs();
        if (mExperimentId >= 0) {
            DbManager::instance().EndExperiment(mExperimentId, endTs);
        }

        shutdownWorker();
        mExperimentId = -1;
        mNextSampleId = -1;
        mExperimentName.clear();
    }

    std::optional<ExperimentRecord> ExperimentParamManager::prepareSample(float fireHeight, float fireArea) {
        if (!isRecording() || mExperimentId < 0) {
            return std::nullopt;
        }

        ExperimentRecord record;
        record.expId = mExperimentId;
        record.sampleId = nextSampleId();
        record.timestampMs = currentTimestampMs();
        record.dist = TFMeaManager::instance().currentDist();
        record.tilt = TFMeaManager::instance().currentTiltAngle();
        record.fireHeight = fireHeight;
        record.fireArea = fireArea;
        record.imagePath = buildImagePath(record.sampleId);

        if (mWorker) {
            mWorker->enqueue(record);
        }

        return record;
    }

    int ExperimentParamManager::nextSampleId() {
        if (mNextSampleId < 0) {
            // channel 无关，只取最新 sample_id
            const auto last = DbManager::instance().GetLastPoint(mExperimentId, kChannelDist);
            mNextSampleId = last ? last->sample_id + 1 : 1;
        }

        return mNextSampleId++;
    }

    void ExperimentParamManager::ensureWorker() {
        if (mWorker && mWorkerThread) {
            return;
        }

        mWorkerThread = new QThread(this);
        mWorker = new ExperimentDbWorker();
        mWorker->moveToThread(mWorkerThread);

        connect(mWorkerThread, &QThread::started, mWorker, &ExperimentDbWorker::startWork);
        connect(mWorkerThread, &QThread::finished, mWorker, &QObject::deleteLater);
        connect(mWorkerThread, &QThread::finished, this, [this]() {
            mWorker = nullptr;
            mWorkerThread = nullptr;
        });
    }

    void ExperimentParamManager::shutdownWorker() {
        if (!mWorkerThread) {
            return;
        }

        if (mWorker) {
            // 使用直接调用避免依赖事件循环（startWork 中未开启事件循环，QueuedConnection 将无法投递并导致线程无法退出）
            QMetaObject::invokeMethod(mWorker, &ExperimentDbWorker::stopWork, Qt::DirectConnection);
        }

        mWorkerThread->quit();
        mWorkerThread->wait();
    }

    QString ExperimentParamManager::buildImagePath(int sampleId) const {
        QDir dir(QDir::currentPath());
        auto base = dir.filePath(QStringLiteral("ai_results"));
        if (mExperimentId >= 0) {
            base = QDir(base).filePath(QStringLiteral("exp_%1").arg(mExperimentId));
        }
        const QString fileName = QStringLiteral("sample_%1.png").arg(sampleId, 6, 10, QLatin1Char('0'));
        return QDir(base).filePath(fileName);
    }

    qint64 ExperimentParamManager::currentTimestampMs() const {
        return QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
}

