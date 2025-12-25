#pragma once

#include <QImage>
#include <QMutex>
#include <QQueue>
#include <QString>
#include <QWaitCondition>
#include <atomic>

#include <opencv2/core.hpp>

#include "TSingleton.h"

namespace TF {

    struct DetectionTask {
        QString sourceFlag;
        cv::Mat image;
        int timeCost{0};
    };

    class DetectionQueueManager : public TBase::TSingleton<DetectionQueueManager> {
    public:
        void start();

        void stop();

        void enqueue(const QString &sourceFlag, const QImage &image, int timeCost);

        bool waitAndPop(DetectionTask &task);

    private:
        friend class TBase::TSingleton<DetectionQueueManager>;

        DetectionQueueManager() = default;

        QMutex mMutex;
        QWaitCondition mCond;
        QQueue<DetectionTask> mTasks;
        std::atomic<bool> mRunning{false};
    };
}

