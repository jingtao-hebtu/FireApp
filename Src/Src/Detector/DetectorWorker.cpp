#include "DetectorWorker.h"
#include "DetectManager.h"
#include "TCvMatQImage.h"
#include "AiResultSaveManager.h"
#include "TLog.h"
#include <QtGlobal>

#include <opencv2/imgproc.hpp>

#include "TFMeaManager.h"


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
        if (task.image.empty()) {
            return;
        }

        const int width = task.image.cols;
        const int height = task.image.rows;
        if (width <= 0 || height <= 0) {
            return;
        }

        cv::Scalar meanScalar = cv::mean(task.image);
        const double mean = (meanScalar[0] + meanScalar[1] + meanScalar[2]) / 3.0;

        QImage preview = QtOcv::mat2Image(task.image);
        emit frameProcessed(task.sourceFlag, preview, mean, task.timeCost);
    }

    void DetectorWorker::processDetect(const DetectionTask& task) {
        if (TFDetectManager::instance().isDetecting()) {

            if (task.image.empty()) {
                return;
            }

            const int width = task.image.cols;
            const int height = task.image.rows;
            if (width <= 0 || height <= 0) {
                return;
            }

            std::chrono::high_resolution_clock::time_point start;
            if (TFDetectManager::instance().needPrintDebugInfo()) {
                start = std::chrono::high_resolution_clock::now();
            }

            cv::Mat cv_im = task.image;
            QImage q_ori = QtOcv::mat2Image(cv_im);

            size_t detect_num = 0;
            std::vector<Detection> detections;
            int detectionId = -1;
            try {
                detectionId = TFDetectManager::instance().runDetectWithPreview(cv_im, detect_num, detections);
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
                std::cout << "Detect time " << duration.count() << " ms, detect_num " << detect_num << std::endl;
            }

            float max_height = 0.0f;
            float max_area = 0.0f;
            for (const auto& detection : detections) {
                auto box_height = static_cast<float>(detection.box.height);
                max_height = max_height > box_height ? max_height : box_height;

                auto area = static_cast<float>(detection.box.height * detection.box.width);
                max_area = max_area > area ? max_area : area;
            }

            TFMeaManager::instance().setFlameDetected(detect_num > 0);
            TFMeaManager::instance().receiveStatistics({
                max_height, max_area,
            });

            // 合成火焰分割掩膜：将所有检测到的火焰mask合并为一张单通道1位图像
            QImage fireMaskImage;
            if (!detections.empty()) {
                cv::Mat combinedMask = cv::Mat::zeros(task.image.rows, task.image.cols, CV_8UC1);
                for (const auto& detection : detections) {
                    if (!detection.mask.empty()) {
                        cv::bitwise_or(combinedMask, detection.mask, combinedMask);
                    }
                }
                // 255->1: 转为1位掩膜
                combinedMask /= 255;

                // 转为 QImage::Format_Mono (1-bit)
                // 先构造 Indexed8，然后转为 Mono
                QImage gray(combinedMask.data, combinedMask.cols, combinedMask.rows,
                            static_cast<int>(combinedMask.step), QImage::Format_Indexed8);
                // 设置调色板：索引0=黑(0), 索引1=白(255)
                QVector<QRgb> colorTable(256);
                colorTable[0] = qRgb(0, 0, 0);
                for (int i = 1; i < 256; ++i)
                    colorTable[i] = qRgb(255, 255, 255);
                gray.setColorTable(colorTable);
                fireMaskImage = gray.convertToFormat(QImage::Format_Mono);
            }

            QImage q_im = QtOcv::mat2Image(cv_im);
            if (detectionId >= 0) {
                AiResultSaveManager::instance().submitResult(q_im, q_ori, fireMaskImage, task.sourceFlag, task.timeCost, detectionId, detect_num,
                                                            max_height, max_area);
            }
            emit frameProcessed(task.sourceFlag, q_im, max_height, task.timeCost);
        }
    }
}

