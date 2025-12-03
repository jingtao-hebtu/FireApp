#include "DetectorWorkerManager.h"

#include <QMetaObject>

#include "DetectionQueueManager.h"

namespace TF {

    DetectorWorkerManager::DetectorWorkerManager(QObject *parent) : QObject(parent) {
    }

    DetectorWorkerManager::~DetectorWorkerManager() {
        stop();
    }

    void DetectorWorkerManager::ensureWorker() {
        if (mWorker) {
            return;
        }

        mThread = new QThread;
        mWorker = new DetectorWorker;
        mWorker->moveToThread(mThread);

        connect(mThread, &QThread::started, mWorker, &DetectorWorker::startWork, Qt::QueuedConnection);
        connect(this, &DetectorWorkerManager::stopWorker, mWorker, &DetectorWorker::stopWork, Qt::QueuedConnection);
        connect(mWorker, &DetectorWorker::frameProcessed, this, &DetectorWorkerManager::frameProcessed,
                Qt::QueuedConnection);
        connect(mThread, &QThread::finished, mWorker, &QObject::deleteLater);
        connect(mThread, &QThread::finished, mThread, &QObject::deleteLater);
    }

    void DetectorWorkerManager::start() {
        ensureWorker();
        if (mRunning.exchange(true)) {
            return;
        }

        if (mThread && !mThread->isRunning()) {
            mThread->start();
        }
    }

    void DetectorWorkerManager::stop() {
        if (!mRunning.exchange(false)) {
            return;
        }

        emit stopWorker();
        DetectionQueueManager::instance().stop();

        if (mThread) {
            mThread->quit();
            mThread->wait();
        }

        mWorker = nullptr;
        mThread = nullptr;
    }

    DetectorWorker *DetectorWorkerManager::worker() const {
        return mWorker;
    }
}

