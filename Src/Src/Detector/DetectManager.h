/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TbDetectManager.h
   Author : tao.jing
   Date   : 2024/11/2
   Brief  :
**************************************************************************/
#ifndef EPDETECTION_TBDETECTMANAGER_H
#define EPDETECTION_TBDETECTMANAGER_H

#include "TSingleton.h"
#include "DetectDef.h"
#include <atomic>
#include <thread>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"


namespace TF {
    struct DetectRet
    {
        int class_id{0};
        float x{0.0f};
        float y{0.0f};
        float w{0.0f};
        float h{0.0f};
    };

    class Detector;

    class TFDetectManager : public TBase::TSingleton<TFDetectManager>
    {
    public:
        ~TFDetectManager();

        bool init();

        void startDetect();

        void stopDetect();

        void pauseDetect();

        bool isDetectPaused() { return mIsPaused; }

        bool isDetecting() { return mIsDetecting; }

        bool isInitialized() { return mInitialized; }

        bool runDetect(const std::string& im_path,
                       std::size_t& detect_num,
                       std::vector<Detection>& detections);

        int runDetect(const cv::Mat& input_frame,
                      std::size_t& detect_num,
                      std::vector<Detection>& detections);

        bool runDetectWithSlice(const cv::Mat& input_frame,
                                std::size_t& detect_num,
                                std::vector<Detection>& detections);

        int runDetectWithPreview(cv::Mat& input_frame,
                                 std::size_t& detect_num,
                                 std::vector<Detection>& detections);

        cv::Scalar generateClassColor(int class_id);

        std::string getDefectNamesByIds(const std::set<int>& class_ids);

        bool needPrintDebugInfo() { return mNeedPrintDebugInfo; }

        bool needSaveOriImg() { return mNeedSaveOriImg; }

    private:
        bool mNeedPrintDebugInfo{false};

        bool mNeedSaveOriImg{false};

        std::atomic<bool> mInitialized{false};
        std::atomic<bool> mIsDetecting{false};

        std::atomic<bool> mIsPaused{false};

        std::atomic<int> mDetectedId{0};

        Detector* mDetector{nullptr};
        std::thread mThread;
    };
};


#endif //EPDETECTION_TBDETECTMANAGER_H
