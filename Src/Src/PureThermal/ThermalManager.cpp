/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : ThermalManager.cpp
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "ThermalManager.h"
#include "ThermalCamera.h"
#include "TConfig.h"
#include "TLog.h"


void TF::ThermalManager::init() {
    mOpenOnInit = GET_BOOL_CONFIG("ThermalCam", "OpenOnInit");
    mWidth = GET_INT_CONFIG("ThermalCam", "Width");
    mHeight = GET_INT_CONFIG("ThermalCam", "Height");
    mFps = GET_INT_CONFIG("ThermalCam", "FPS");
}

void TF::ThermalManager::initAfterWid() {
    if (mOpenOnInit) {
        start();
    }
}

void TF::ThermalManager::start() {
    if (mStarted.load()) {
        return;
    }

    if (!mThermalCamera) {
        return;
    }

    bool ret = mThermalCamera->start(mWidth, mHeight, mFps);
    if (!ret) {
        LOG_F(ERROR, "Thermal camera failed to start");
    }

    mStarted.store(true);
}
