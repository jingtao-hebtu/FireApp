/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : Detector.h
   Author : tao.jing
   Date   : 2024/11/2
   Brief  : 
**************************************************************************/
#ifndef EPDETECTION_TFDETECTOR_H
#define EPDETECTION_TFDETECTOR_H

#include "DetectDef.h"
#include <string>



namespace TF {

    class InferenceORT;

    class Detector {

    public:
        Detector();

        bool init();

        bool runDetect(const std::string &im_path,
               size_t &detect_num,
               std::vector<Detection> &detections,
               cv::Mat &frame);

        bool runDetectWithPreview(cv::Mat &input_frame,
                                  size_t &detect_num,
                                  std::vector<Detection> &detections);

        bool runDetect(const cv::Mat &input_frame,
                       size_t &detect_num,
                       std::vector<Detection> &detections);

    private:
        bool mRunOnGPU{true};
        InferenceORT *mInfORT{nullptr};
    };

};


#endif //EPDETECTION_TBDETECTOR_H
