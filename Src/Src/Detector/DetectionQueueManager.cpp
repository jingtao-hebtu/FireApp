#include "DetectionQueueManager.h"

#include <QMutexLocker>

namespace TF {

    namespace {
        const int MaxQueueSize = 5;
    }

    void DetectionQueueManager::start() {
        mRunning.store(true);
    }

    void DetectionQueueManager::stop() {
        mRunning.store(false);
        {
            QMutexLocker locker(&mMutex);
            mTasks.clear();
        }
        mCond.wakeAll();
    }

    void DetectionQueueManager::enqueue(const QString &sourceFlag, const QImage &image, int timeCost) {
        if (!mRunning.load()) {
            return;
        }

        QMutexLocker locker(&mMutex);
        if (mTasks.size() >= MaxQueueSize) {
            mTasks.dequeue();
        }

        mTasks.enqueue({sourceFlag, image, timeCost});
        mCond.wakeOne();
    }

    bool DetectionQueueManager::waitAndPop(DetectionTask &task) {
        QMutexLocker locker(&mMutex);
        while (mRunning.load() && mTasks.isEmpty()) {
            mCond.wait(&mMutex);
        }

        if (!mRunning.load()) {
            return false;
        }

        task = mTasks.dequeue();
        return true;
    }
}

