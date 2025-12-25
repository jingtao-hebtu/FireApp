/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : InferenceTRT.cpp
   Author : tao.jing
   Date   : 2025.12.24
   Brief  :
**************************************************************************/
#include "InferenceTRT.h"
#include "TConfig.h"


TF::InferenceTRT::InferenceTRT() {
}

TF::InferenceTRT::~InferenceTRT() {
}

void TF::InferenceTRT::init() {
    mEnginePath = GET_STR_CONFIG("TrtYolo", "EnginePath");

    mOption.enableSwapRB();
    mModel = std::make_unique<trtyolo::SegmentModel>(mEnginePath, mOption);
}

void TF::InferenceTRT::runInference(const cv::Mat& input) {
    cv::resize(input, input, cv::Size(640, 640));
    trtyolo::Image img(input.data, input.cols, input.rows);
}
