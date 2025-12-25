/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : InferenceTRT.h
   Author : tao.jing
   Date   : 2025.12.24
   Brief  :
**************************************************************************/
#ifndef FIREAPP_INFERENCETRT_H
#define FIREAPP_INFERENCETRT_H

#include <atomic>
#include <opencv2/opencv.hpp>

#include "trtyolo.hpp"


namespace TF {
    class InferenceTRT
    {
    public:
        InferenceTRT();

        ~InferenceTRT();

    private:
        void init();

    public:
        void runInference(const cv::Mat &input);

    private:
        std::atomic<bool> mInitialized {false};

        std::string mEnginePath {};

        trtyolo::InferOption mOption;
        std::unique_ptr<trtyolo::SegmentModel> mModel;

    };
};


#endif //FIREAPP_INFERENCETRT_H
