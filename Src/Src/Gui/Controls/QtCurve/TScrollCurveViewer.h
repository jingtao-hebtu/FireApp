/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TScrollCurveViewer.h
   Author : tao.jing
   Date   : 2023/11/29
   Brief  :
**************************************************************************/
#ifndef QONEDARK_SCROLLCURVEVIEWER_H
#define QONEDARK_SCROLLCURVEVIEWER_H

#include "TBaseCurveViewer.h"
#include <cstdint>
#include "TaoUtilDefine.h"


namespace T_QtBase {

    class TCurveViewer_Ui;

    class TScrollCurveViewer : public TBaseCurveViewer {

        Q_OBJECT
    public:
        explicit TScrollCurveViewer(std::uint8_t x_left_staging = 0);

        void updateXAxisRange(std::int32_t x_min, std::int32_t x_max) final;
        void updateYAxisRange(std::int32_t y_min, std::int32_t y_max) final;

    signals:
        void receiveCurvePoint(QPointF);
        void receiveNextValue(float);

    public slots:
        void onReceiveCurvePoint(QPointF point);
        void onReceiveNextValue(float value);

    private:
        void initAction();
        void removePointsByXThr(std::uint32_t x_thr);

    private:
        std::uint8_t mXLeftStaging {10};
        TCurveViewer_Ui* mUi {nullptr};
    };

};



#endif //QONEDARK_SCROLLCURVEVIEWER_H
