#include "DetectorWorker.h"

#include <QImage>
#include <QtGlobal>

namespace TF {

    DetectorWorker::DetectorWorker(QObject *parent) : QObject(parent) {
    }

    void DetectorWorker::startWork() {
        mRunning.store(true);
        DetectionQueueManager::instance().start();

        DetectionTask task;
        while (mRunning.load()) {
            if (!DetectionQueueManager::instance().waitAndPop(task)) {
                break;
            }
            processFrame(task);
        }
    }

    void DetectorWorker::stopWork() {
        mRunning.store(false);
        DetectionQueueManager::instance().stop();
    }

    void DetectorWorker::processFrame(const DetectionTask &task) {
        if (task.image.isNull()) {
            return;
        }

        QImage converted = task.image.convertToFormat(QImage::Format_ARGB32);
        const int width = converted.width();
        const int height = converted.height();
        if (width <= 0 || height <= 0) {
            return;
        }

        quint64 graySum = 0;
        for (int y = 0; y < height; ++y) {
            const QRgb *line = reinterpret_cast<const QRgb *>(converted.constScanLine(y));
            for (int x = 0; x < width; ++x) {
                graySum += qGray(line[x]);
            }
        }

        const int pixelCount = width * height;
        const double mean = pixelCount > 0 ? static_cast<double>(graySum) / pixelCount : 0.0;

        emit frameProcessed(task.sourceFlag, task.image, mean, task.timeCost);
    }
}

