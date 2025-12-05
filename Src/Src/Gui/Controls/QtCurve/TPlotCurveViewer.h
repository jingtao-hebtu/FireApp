/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : PlotCurveViewer.h
   Author : tao.jing
   Date   : 2023/12/3
   Brief  : 
**************************************************************************/
#ifndef QONEDARK_PLOTCURVEVIEWER_H
#define QONEDARK_PLOTCURVEVIEWER_H

#include "TBaseCurveViewer.h"
#include "TaoUtilDefine.h"


namespace T_QtBase {

    class TCurveViewer_Ui;

    class TPlotCurveViewer : public TBaseCurveViewer {

        Q_OBJECT
    public:
        explicit TPlotCurveViewer();

        void updateXAxisRange(std::int32_t x_min, std::int32_t x_max) final;
        void updateYAxisRange(std::int32_t y_min, std::int32_t y_max) final;

    signals:
        void receiveCurvePoint(const QPointF& point);

    private:
        void initAction();
        void validatePointXValue(float x);
        void validatePointYValue(float y);

    public slots:
        void onReceiveCurvePoint(const QPointF& point);

    private:
        TCurveViewer_Ui* mUi {nullptr};
    };

};



#endif //QONEDARK_PLOTCURVEVIEWER_H
