#include "DetectorWorker.h"
#include "DetectManager.h"
#include "TCvMatQImage.h"
#include "TLog.h"
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
            //processFrame(task);
            processDetect(task);
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

    void DetectorWorker::processDetect(const DetectionTask& task) {
        if (TFDetectManager::instance().isDetecting()) {

            std::chrono::high_resolution_clock::time_point start;
            if (TFDetectManager::instance().needPrintDebugInfo()) {
                start = std::chrono::high_resolution_clock::now();
            }

            cv::Mat cv_im = QtOcv::image2Mat(task.image, CV_8UC3).clone();

            size_t detect_num = 0;
            std::vector<Detection> detections;
            try {
                TFDetectManager::instance().runDetectWithPreview(cv_im, detect_num, detections);
            } catch (const cv::Exception &e) {
                LOG_F(ERROR, "Object detect inference failed: %s.", e.what());
            } catch (const std::exception &e) {
                LOG_F(ERROR, "Object detect inference failed: %s.", e.what());
            } catch (...) {
                LOG_F(ERROR, "Object detect inference failed.");
            }

            if (TFDetectManager::instance().needPrintDebugInfo()) {
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                std::cout << "Detect time " << duration << " ms, detect_num " << detect_num << std::endl;
            }

            float max_height = 0.0f;
            for (const auto& detection : detections) {
                auto box_height = static_cast<float>(detection.box.height);
                max_height = max_height > box_height ? max_height : box_height;
            }

            QImage q_im = QtOcv::mat2Image(cv_im);
            emit frameProcessed(task.sourceFlag, q_im, max_height, task.timeCost);

        }
    }
}

