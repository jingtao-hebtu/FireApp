/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : Detector.cpp
   Author : tao.jing
   Date   : 2024/11/2
   Brief  :
**************************************************************************/
#include "Detector.h"
#include "InferenceORT.h"
#include "TSysUtils.h"
#include "TConfig.h"
#include "TLog.h"

#include <opencv2/opencv.hpp>


namespace TF {
    Detector::Detector() {
    }

    bool Detector::init() {
        auto model_path = GET_STR_CONFIG("VisionMea", "DetModelName");
        LOG_F(INFO, "Load detection model %s.", model_path.c_str());

        if (!TBase::fileExists(model_path)) {
            LOG_F(ERROR, "Model file %s not exists.", model_path.c_str());
        }

        try {
            mInfORT = new InferenceORT(model_path);
            LOG_F(INFO, "[TbDetector] TbInferenceORT model loaded.");
            return true;
        }
        catch (const std::exception& ex) {
            LOG_F(ERROR, "[TbDetector] TbInference model load failed, %s.", ex.what());
        }
        catch (const std::string& ex) {
            LOG_F(ERROR, "[TbDetector] TbInference model load failed, %s.", ex.c_str());
        }
        catch (...) {
            LOG_F(ERROR, "[TbDetector] TbInference model load failed, unknown exception.");
        }
        return false;
    }

    bool Detector::runDetect(const std::string& im_path, size_t& detect_num, std::vector<Detection>& detections,
        cv::Mat& frame) {
        if (!TBase::fileExists(im_path)) {
            LOG_F(ERROR, "Image path %s not exists.", im_path.c_str());
            return false;
        }

        if (mInfORT == nullptr) {
            LOG_F(ERROR, "TbInference not init.");
            return false;
        }

        try {
            frame = cv::imread(im_path);

            detections = mInfORT->runInference(frame);
            detect_num = detections.size();

            for (auto idx = 0; idx < detect_num; idx++) {
                const auto &detection = detections[idx];
                cv::Rect box = detection.box;
                cv::Scalar color = detection.color;
                cv::rectangle(frame, box, color, 2);

                std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
                cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
                cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

                cv::rectangle(frame, textBox, color, cv::FILLED);
                cv::putText(frame, classString,
                            cv::Point(box.x + 5, box.y - 10),
                            cv::FONT_HERSHEY_DUPLEX, 1,
                            cv::Scalar(0, 0, 0), 2, 0);
            }
            return true;
        } catch (const std::exception &ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.what());
        } catch (const std::string &ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.c_str());
        } catch (...) {
            LOG_F(ERROR, "[TbDetector] Run inference failed");
        }
        return false;
    }

    bool Detector::runDetect(const cv::Mat& input_frame,
                             size_t& detect_num,
                             std::vector<Detection>& detections) {
        if (input_frame.empty()) {
            return false;
        }

        detections = mInfORT->runInference(input_frame);
        detect_num = detections.size();
        return true;
    }

    bool Detector::runDetectWithPreview(cv::Mat& input_frame,
                                        size_t& detect_num,
                                        std::vector<Detection>& detections) {
        if (input_frame.empty()) {
            return false;
        }

        try {
            detections = mInfORT->runInference(input_frame);
            detect_num = detections.size();

            for (auto idx = 0; idx < detect_num; idx++) {
                const auto& detection = detections[idx];

                cv::Rect box = detection.box;
                cv::Scalar color = detection.color;
                cv::rectangle(input_frame, box, color, 4);
                /*
                std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
                cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
                cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

                cv::rectangle(input_frame, textBox, color, cv::FILLED);
                cv::putText(input_frame, classString,
                            cv::Point(box.x + 5, box.y - 10),
                            cv::FONT_HERSHEY_DUPLEX, 1,
                            cv::Scalar(0, 0, 0), 2, 0);*/
            }
            return true;
        }
        catch (const std::exception& ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.what());
        }
        catch (const std::string& ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.c_str());
        }
        catch (...) {
            LOG_F(ERROR, "[TbDetector] Run inference failed");
        }
        return false;
    }
};
