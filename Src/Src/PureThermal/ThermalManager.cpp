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
#include "TFException.h"
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
        try {
            start();
        } catch (TFException& e) {
            LOG_F(ERROR, "红外模组初始化失败, 请检查模块连接. %s", e.what());
        }
    }
}

void TF::ThermalManager::start() {
    if (mStarted.load()) {
        TF_LOG_THROW_PROMPT("红外模块已经启动.");
    }

    if (!mThermalCamera) {
        TF_LOG_THROW_PROMPT("ThermalCamera未初始化.");
    }

    try {
        bool ret = mThermalCamera->start(mWidth, mHeight, mFps);
        if (!ret) {
            TF_LOG_THROW_PROMPT("红外模组初始化失败, 请检查模块连接.");
        }
    } catch (std::exception &e) {
        TF_LOG_THROW_PROMPT("红外模组初始化失败, 请检查模块连接. %s", e.what());
    } catch (...) {
        TF_LOG_THROW_PROMPT("红外模组初始化失败, 请检查模块连接. %s");
    }
    mStarted.store(true);
}

void TF::ThermalManager::stop() {
    if (!mStarted.load()) {
        TF_LOG_THROW_PROMPT("红外模块已经关闭.");
        return;
    }

    if (!mThermalCamera) {
        TF_LOG_THROW_PROMPT("ThermalCamera未初始化.");
        return;
    }

    try {
        mThermalCamera->stop();
    } catch (std::exception &e) {
        TF_LOG_THROW_PROMPT("红外模组关闭失败, 请检查模块连接. %s", e.what());
    } catch (...) {
        TF_LOG_THROW_PROMPT("红外模组关闭失败, 请检查模块连接.");
    }
    mStarted.store(false);
}
