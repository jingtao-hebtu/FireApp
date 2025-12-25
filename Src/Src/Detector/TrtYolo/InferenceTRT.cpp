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
#include "TLog.h"


TF::InferenceTRT::InferenceTRT() {
    init();
}

TF::InferenceTRT::~InferenceTRT() {
}

void TF::InferenceTRT::init() {
    mEnginePath = GET_STR_CONFIG("TrtYolo", "EnginePath");

    mOption.enableSwapRB();
    mModel = std::make_unique<trtyolo::SegmentModel>(mEnginePath, mOption);
}

trtyolo::SegmentRes TF::InferenceTRT::runInference(const cv::Mat& input) {
    auto frame = input.clone();
    cv::resize(frame, frame, cv::Size(640, 640));
    trtyolo::Image img(frame.data, frame.cols, frame.rows);

    auto result = mModel->predict(img);
    return result;
}
