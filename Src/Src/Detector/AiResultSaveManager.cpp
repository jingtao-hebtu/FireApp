#include "AiResultSaveManager.h"

#include <algorithm>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>

#include "DetectManager.h"
#include "TLog.h"
#include "ExperimentParamManager.h"

namespace TF {

    void AiResultSaveWorker::enqueue(const QImage &image, const QString &filePath, const QString &description) {
        if (image.isNull()) {
            return;
        }

        QMutexLocker locker(&mMutex);
        mTasks.enqueue({image.copy(), filePath, description});
        mCond.wakeOne();
    }

    void AiResultSaveWorker::startWork() {
        mRunning.store(true);

        while (mRunning.load()) {
            Task task;
            {
                QMutexLocker locker(&mMutex);
                if (mTasks.isEmpty()) {
                    mCond.wait(&mMutex);
                }

                if (!mRunning.load()) {
                    break;
                }

                if (mTasks.isEmpty()) {
                    continue;
                }

                task = mTasks.dequeue();
            }

            if (task.image.isNull() || task.filePath.isEmpty()) {
                continue;
            }

            QDir dir(QFileInfo(task.filePath).absolutePath());
            if (!dir.exists()) {
                dir.mkpath(".");
            }

            if (!task.image.save(task.filePath)) {
                LOG_F(ERROR, "Failed to save AI result image to %s", task.filePath.toStdString().c_str());
                continue;
            }

            LOG_F(INFO, "Saved AI result image: %s | %s", task.filePath.toStdString().c_str(),
                  task.description.toStdString().c_str());
        }

        QMutexLocker locker(&mMutex);
        mTasks.clear();
    }

    void AiResultSaveWorker::stopWork() {
        mRunning.store(false);
        mCond.wakeAll();
    }

    AiResultSaveManager::AiResultSaveManager(QObject *parent) : QObject(parent) {
    }

    void AiResultSaveManager::ensureWorker() {
        if (mWorker) {
            return;
        }

        mThread = new QThread;
        mWorker = new AiResultSaveWorker;
        mWorker->moveToThread(mThread);

        connect(mThread, &QThread::started, mWorker, &AiResultSaveWorker::startWork, Qt::QueuedConnection);
        connect(this, &AiResultSaveManager::stopWorker, mWorker, &AiResultSaveWorker::stopWork, Qt::QueuedConnection);
        connect(mThread, &QThread::finished, mWorker, &QObject::deleteLater);
        connect(mThread, &QThread::finished, mThread, &QObject::deleteLater);
    }

    void AiResultSaveManager::setEnabled(bool enabled) {
        if (enabled) {
            ensureWorker();

            if (mEnabled.exchange(true)) {
                return;
            }

            resetFrequencyWindow();

            if (mThread && !mThread->isRunning()) {
                mThread->start();
            }
            return;
        }

        if (!mEnabled.exchange(false)) {
            return;
        }

        if (mWorker) {
            mWorker->stopWork();
        }
        if (mThread) {
            mThread->quit();
            mThread->wait();
        }

        mWorker = nullptr;
        mThread = nullptr;
    }

    void AiResultSaveManager::setSaveFrequency(int imagesPerSecond) {
        const int sanitized = std::max(1, imagesPerSecond);
        mSaveFrequency.store(sanitized);
        resetFrequencyWindow();
    }

    bool AiResultSaveManager::shouldSaveNow() {
        QMutexLocker locker(&mFreqMutex);
        if (!mFrequencyTimer.isValid() || mFrequencyTimer.elapsed() >= 1000) {
            mFrequencyTimer.restart();
            mSavedInWindow = 0;
        }

        if (mSavedInWindow >= mSaveFrequency.load()) {
            return false;
        }

        ++mSavedInWindow;
        return true;
    }

    void AiResultSaveManager::resetFrequencyWindow() {
        QMutexLocker locker(&mFreqMutex);
        mFrequencyTimer.restart();
        mSavedInWindow = 0;
    }

    void AiResultSaveManager::recordMeta(const QString &path, const QString &description) {
        QMutexLocker locker(&mRecordMutex);
        constexpr std::size_t MaxRecordCount = 50;
        mRecentRecords.push_back({path, description, QDateTime::currentDateTime()});
        if (mRecentRecords.size() > MaxRecordCount) {
            mRecentRecords.erase(mRecentRecords.begin(), mRecentRecords.begin() + (mRecentRecords.size() - MaxRecordCount));
        }
    }

    std::vector<AiResultMetaInfo> AiResultSaveManager::recentRecords() const {
        QMutexLocker locker(&mRecordMutex);
        return mRecentRecords;
    }

    void AiResultSaveManager::submitResult(const QImage &image,
                                           const QString &sourceFlag,
                                           int timeCost,
                                           int detectionId,
                                           std::size_t detectedCount,
                                           float fireHeight,
                                           float fireArea) {
        if (!mEnabled.load()) {
            return;
        }

        if (!TFDetectManager::instance().isDetecting()) {
            return;
        }

        if (!shouldSaveNow()) {
            return;
        }

        auto record = ExperimentParamManager::instance().prepareSample(fireHeight, fireArea);
        if (!record.has_value()) {
            return;
        }

        ensureWorker();
        if (mThread && !mThread->isRunning()) {
            mThread->start();
        }

        if (!mWorker) {
            return;
        }

        const QString filePath = record->imagePath.isEmpty()
            ? QDir(QDir::currentPath()).filePath("ai_results/" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") + ".png")
            : record->imagePath;

        const QString description = QString("flag:%1 detectId:%2 count:%3 cost:%4ms")
                                        .arg(sourceFlag)
                                        .arg(detectionId)
                                        .arg(detectedCount)
                                        .arg(timeCost);

        mWorker->enqueue(image, filePath, description);
        recordMeta(filePath, description);
    }
}

