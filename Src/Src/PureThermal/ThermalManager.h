/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : ThermalManager.h
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREAPP_THERMALMANAGER_H
#define FIREAPP_THERMALMANAGER_H

#include "ThermalCamera.h"
#include "TSingleton.h"
#include <atomic>


namespace TF {

    class ThermalCamera;

    class ThermalManager : public TBase::TSingleton<ThermalManager>
    {
    public:
        void init();
        void initAfterWid();
        void start();

        void setThermalCamera(ThermalCamera *camera) {mThermalCamera = camera;}
        ThermalCamera * getThermalCamera() {return mThermalCamera;}

    private:
        std::atomic<bool> mStarted {false};

        bool mOpenOnInit {false};
        int mWidth {160};
        int mHeight {120};
        int mFps {9};

        ThermalCamera *mThermalCamera {nullptr};
    };
};


#endif //FIREAPP_THERMALMANAGER_H
