/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : BmsData.h
   Author : tao.jing
   Date   : 2025.12.27
   Brief  :
**************************************************************************/
#ifndef FIREAPP_BMSDATA_H
#define FIREAPP_BMSDATA_H

#include <QDateTime>


struct BmsStatus
{
    QDateTime mTimestamp;

    double mTotalVoltage_V = 0.0;
    double mCurrent_A = 0.0;
    double mRemainCapacity_Ah = 0.0;
    double mTemp1_C = 0.0;
    double mTemp2_C = 0.0;

    bool ok = false;
    QString error;
};


#endif //FIREAPP_BMSDATA_H
