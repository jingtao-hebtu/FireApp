#pragma once

#include <QImage>
#include <QString>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <atomic>
#include "TSingleton.h"

namespace TF {

    struct DetectionTask {
        QString sourceFlag;
        QImage image;
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

