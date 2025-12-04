/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : DetectDef.h
   Author : tao.jing
   Date   : 2024/11/2
   Brief  :
**************************************************************************/
#ifndef EPDETECTION_DETECTDEF_H
#define EPDETECTION_DETECTDEF_H

#include <opencv2/opencv.hpp>

namespace TF {

    enum TF_DETECT_CLASS_ID {
        TF_INTRODUCTION_DEVICE = 0,
        TF_INTRODUCTION_DEVICE_NOT_SEALED,
        TF_INTRODUCTION_DEVICE_MULTIPLE_ENTRIES,
        TF_FASTENING_DEVICE,
        TF_FASTENING_LOOSE,
        TF_FASTENING_DEVICE_MISSING,
        TF_GROUND_WIRE_LABEL,
        TF_GROUND_WIRE,
        TF_TAR_BOX,
        TF_CLASS_NUM
    };

    static const char* TF_DEFECT_CLASS_NAMES[TF_CLASS_NUM] = {
        "引入装置",
        "引入装置未封堵",
        "引入装置多进一",
        "紧固装置",
        "紧固装置松弛",
        "紧固装置缺失",
        "接地标识",
        "接地地线",
        "防爆设备",
    };

    static const int TF_DETECT_IMG_SIZE = 640;

    typedef struct _Detection {
        int class_id{0};
        std::string className{};
        float confidence{0.0};
        cv::Scalar color{};
        cv::Rect box{};
        cv::Mat mask{};
    } Detection;

};


#endif //EPDETECTION_TBDETECTDEF_H
