#pragma once

#include <QObject>
#include <atomic>

#include "DetectionQueueManager.h"

namespace TF {

    class DetectorWorker : public QObject {
    Q_OBJECT

    public:
        explicit DetectorWorker(QObject *parent = nullptr);

        ~DetectorWorker() override = default;

    signals:
        void frameProcessed(const QString &sourceFlag, const QImage &image, double meanValue, int timeCost);

    public slots:
        void startWork();

        void stopWork();

    private:
        void processFrame(const DetectionTask &task);

        std::atomic<bool> mRunning{false};
    };
}

