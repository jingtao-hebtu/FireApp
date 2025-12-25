#include "DetectionQueueManager.h"

#include <QMutexLocker>

#include "TCvMatQImage.h"

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

        if (image.isNull() || image.width() <= 0 || image.height() <= 0) {
            return;
        }

        cv::Mat safeMat = QtOcv::image2Mat(image, CV_8UC3).clone();
        if (safeMat.empty()) {
            return;
        }

        QMutexLocker locker(&mMutex);
        if (mTasks.size() >= MaxQueueSize) {
            mTasks.dequeue();
        }

        // Store a copied cv::Mat to avoid referencing buffers that might be
        // released once playback stops, while skipping a second deep copy
        // when the worker converts the frame.
        mTasks.enqueue({sourceFlag, std::move(safeMat), timeCost});
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

