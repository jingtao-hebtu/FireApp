/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TPlotCurveViewer.cpp
   Author : tao.jing
   Date   : 2023/12/3
   Brief  : 
**************************************************************************/
#include "TPlotCurveViewer.h"
#include "TCurveViewer_Ui.h"
#include "TException.h"
#include <QValueAxis>
#include <QXYSeries>


namespace T_QtBase {

    TPlotCurveViewer::TPlotCurveViewer() {
        mUi = new TCurveViewer_Ui();
        mUi->setupUi(this);
        initAction();
    }

    void TPlotCurveViewer::initAction() {
        connect(this, SIGNAL(receiveCurvePoint(const QPointF&)), this, SLOT(onReceiveCurvePoint(const QPointF&)));
    }

    void TPlotCurveViewer::updateXAxisRange(std::int32_t x_min, std::int32_t x_max) {
        mXMin = x_min;
        mXMax = x_max;
        mUi->mAxisX->setRange(x_min, x_max);
    }

    void TPlotCurveViewer::updateYAxisRange(std::int32_t y_min, std::int32_t y_max) {
        mYMin = y_min;
        mYMax = y_max;
        mUi->mAxisY->setRange(y_min, y_max);
    }

    void TPlotCurveViewer::onReceiveCurvePoint(const QPointF& point) {
        validatePointXValue(static_cast<float>(point.x()));
        validatePointYValue(static_cast<float>(point.y()));
        mUi->mCurveSeries->append(point);
    }

    void TPlotCurveViewer::validatePointXValue(float x) {
        auto x_min = getXMin();
        auto x_max = getXMax();
        if (x < x_min || x > x_max) {
            TBASE_LOG_THROW_RUNTIME("Invalid point x() %f, for x_min %d and x_max %d.", x, x_min, x_max);
        }
    }

    void TPlotCurveViewer::validatePointYValue(float y) {
        auto y_min = getYMin();
        auto y_max = getYMax();
        if (y < y_min || y > y_max) {
            TBASE_LOG_THROW_RUNTIME("Invalid point y() %f, for y_min %d and y_max %d.", y, y_min, y_max);
        }
    }




};

