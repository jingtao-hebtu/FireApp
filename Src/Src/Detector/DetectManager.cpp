/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TbDetectManager.cpp
   Author : tao.jing
   Date   : 2024/11/2
   Brief  :
**************************************************************************/
#include "DetectManager.h"
#include "Inference/Detector.h"
#include "Inference/InferenceORT.h"
#include "TSysUtils.h"
#include "TLog.h"
#include "TCvMatQImage.h"
#include "TConfig.h"
#include <chrono>


namespace TF {
    void detectLoop() {
        while (TFDetectManager::instance().isDetecting()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }


    TFDetectManager::~TFDetectManager() {
        stopDetect();
    }

    bool TFDetectManager::init() {
        if (mInitialized) {
            LOG_F(INFO, "[TbDetectManager::init] TbDetectManager already initialized.");
            return true;
        }

        mNeedPrintDebugInfo = GET_BOOL_CONFIG("RGBCam", "NeedPrintDebugInfo");
        mNeedSaveOriImg = GET_BOOL_CONFIG("RGBCam", "NeedSaveOriImg");

        mDetector = new Detector();
        mInitialized = mDetector->init();
        LOG_F(INFO, "TbDetectManager init finished, ret %d.", static_cast<int>(mInitialized));
        return mInitialized;
    }

    void TFDetectManager::startDetect() {
        if (!mInitialized) {
            LOG_F(ERROR, "[TbDetectManager::onStartDetectBtnClicked] TbDetectManager not initialized.");
            return;
        }

        mIsPaused.store(false); // Restore detect if paused
        if (mIsDetecting) {
            return;
        }

        mIsDetecting = true;
        //mThread = std::thread(detectLoop);
    }

    void TFDetectManager::stopDetect() {
        if (!mInitialized) {
            LOG_F(ERROR, "[TbDetectManager::onStopDetectBtnClicked] TbDetectManager not initialized.");
            return;
        }

        mIsDetecting = false;
        /*
        if (mThread.joinable()) {
            mThread.join();
        }
        */
    }

    void TFDetectManager::pauseDetect() {
        mIsPaused.store(true);
        // Keep detect ret processing for left results
        //TbDetectRetManager::instance().pause();
    }

    bool TFDetectManager::runDetect(const std::string& im_path,
                                    std::size_t& detect_num,
                                    std::vector<Detection>& detections) {
        if (mDetector == nullptr) {
            return false;
        }

        cv::Mat frame;
        bool ret = mDetector->runDetect(im_path, detect_num, detections, frame);
        return ret;
    }

    int TFDetectManager::runDetect(const cv::Mat& input_frame,
                                   size_t& detect_num,
                                   std::vector<Detection>& detections) {
        if (mDetector == nullptr) {
            return false;
        }

        bool ret = mDetector->runDetect(input_frame, detect_num, detections);
        if (ret) {
            return ++mDetectedId;
        }
        return -1;
    }

    bool TFDetectManager::runDetectWithSlice(const cv::Mat& input_frame, size_t& detect_num,
                                             std::vector<Detection>& detections) {
        if (mDetector == nullptr) {
            return false;
        }

        bool ret = mDetector->runDetect(input_frame, detect_num, detections);
        return ret;
    }

    int TFDetectManager::runDetectWithPreview(cv::Mat& input_frame,
                                              std::size_t& detect_num,
                                              std::vector<Detection>& detections) {
        if (mDetector == nullptr) {
            return false;
        }

        bool ret = mDetector->runDetectWithPreview(input_frame, detect_num, detections);
        if (ret) {
            mDetectedId = mDetectedId + 1;
            return mDetectedId;
        }
        return -1;
    }

    cv::Scalar CLASS_COLORS[TF_CLASS_NUM] = {
        cv::Scalar(0, 255, 0), // TB_INTRODUCTION_DEVICE 0
        cv::Scalar(0, 0, 255), // TB_INTRODUCTION_DEVICE_NOT_SEALED 1
        cv::Scalar(0, 100, 255), // TB_INTRODUCTION_DEVICE_MULTIPLE_ENTRIES 2
        cv::Scalar(0, 255, 255), // TB_FASTENING_DEVICE 3
        cv::Scalar(0, 50, 150), // TB_FASTENING_LOOSE 4
        cv::Scalar(0, 50, 200), // TB_FASTENING_DEVICE_MISSING 5
        cv::Scalar(255, 0, 0), // TB_GROUND_WIRE_LABEL 6
        cv::Scalar(255, 255, 0), // TB_GROUND_WIRE 7
        cv::Scalar(255, 0, 255) // TB_TAR_BOX 8
    };

    cv::Scalar TFDetectManager::generateClassColor(int class_id) {
        if (class_id >= TF_CLASS_NUM) {
            LOG_F(ERROR, "Invalid class id for color generation %d.", class_id);
        }
        return CLASS_COLORS[class_id];
    }

    std::string TFDetectManager::getDefectNamesByIds(const std::set<int>& class_ids) {
        std::string defect_names;
        for (auto it = class_ids.begin(); it != class_ids.end(); ++it) {
            if (*it < 0 || *it >= TF_CLASS_NUM) {
                LOG_F(ERROR, "Invalid defect class id %d.", *it);
                continue;
            }
            if (it == class_ids.begin()) {
                defect_names += (TF_DEFECT_CLASS_NAMES[*it]);
            }
            else {
                defect_names += (std::string(", ") + TF_DEFECT_CLASS_NAMES[*it]);
            }
        }
        return defect_names;
    }
};
