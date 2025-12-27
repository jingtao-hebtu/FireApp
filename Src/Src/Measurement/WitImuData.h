/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : WitImuData.h
   Author : tao.jing
   Date   : 2025.12.26
   Brief  :
**************************************************************************/
#ifndef FIREAPP_WITIMUDATA_H
#define FIREAPP_WITIMUDATA_H


struct Vec3f
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct WitImuData
{
    Vec3f acc; // g
    Vec3f gyro; // deg/s
    Vec3f angle; // deg
    Vec3f mag; // raw

    bool accValid = false;
    bool gyroValid = false;
    bool angleValid = false;
    bool magValid = false;

    long long timestampMs = 0;
};

#endif //FIREAPP_WITIMUDATA_H