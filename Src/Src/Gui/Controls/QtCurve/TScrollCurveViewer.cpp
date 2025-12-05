/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TScrollCurveViewer.cpp
   Author : tao.jing
   Date   : 2023/11/29
   Brief  : 
**************************************************************************/
#include "TScrollCurveViewer.h"
#include "TCurveViewer_Ui.h"
#include <QXYSeries>
#include <QValueAxis>


namespace T_QtBase {

    TScrollCurveViewer::TScrollCurveViewer(std::uint8_t x_left_staging) : mXLeftStaging(x_left_staging) {
        mUi = new TCurveViewer_Ui();
        mUi->setupUi(this);
        initAction();
    }

    void TScrollCurveViewer::initAction() {
        connect(this, SIGNAL(receiveCurvePoint(QPointF)), this, SLOT(onReceiveCurvePoint(QPointF)));
        connect(this, SIGNAL(receiveNextValue(float)), this, SLOT(onReceiveNextValue(float)));
    }

    void TScrollCurveViewer::updateXAxisRange(std::int32_t x_min, std::int32_t x_max) {
        mXMin = x_min;
        mXMax = x_max;
        mUi->mAxisX->setRange(x_min, x_max);
    }

    void TScrollCurveViewer::updateYAxisRange(std::int32_t y_min, std::int32_t y_max) {
        mYMin = y_min;
        mYMax = y_max;
        mUi->mAxisY->setRange(y_min, y_max);
    }

    void TScrollCurveViewer::onReceiveCurvePoint(QPointF point) {
        auto points = mUi->mCurveSeries->points();
        if (points.empty()) {
            mUi->mCurveSeries->append(point);
            return;
        }
        mUi->mCurveSeries->append(point);

        qreal axis_x_max = dynamic_cast<QValueAxis*>(mUi->mChart->axes(Qt::Horizontal).first())->max();
        if (axis_x_max < point.x() + 1) {
            mUi->mChart->scroll(point.x() + 1 - axis_x_max, 0);
        }

        auto x_start = points[0].x();
        qreal axis_x_min = dynamic_cast<QValueAxis*>(mUi->mChart->axes(Qt::Horizontal).first())->min();
        if (axis_x_min > x_start + mXLeftStaging) {
            removePointsByXThr(static_cast<std::uint32_t>(x_start + mXLeftStaging));
        }
    }

    void TScrollCurveViewer::removePointsByXThr(std::uint32_t x_thr) {
        auto points = mUi->mCurveSeries->points();
        std::uint16_t remove_cnt = 0;
        for (auto point : points) {
            if (point.x() > x_thr) {
                break;
            }
            remove_cnt ++;
        }
        mUi->mCurveSeries->removePoints(0, remove_cnt);
    }

    void TScrollCurveViewer::onReceiveNextValue(float value) {
        auto points = mUi->mCurveSeries->points();
        QPointF point;
        if (points.empty()) {
            qreal axis_x_min = dynamic_cast<QValueAxis*>(mUi->mChart->axes(Qt::Horizontal).first())->min();
            point = QPointF(axis_x_min, value);
            emit receiveCurvePoint(point);
            return;
        } else {
            qreal x_step = 1.0;
            qreal x_end = points.back().x();
            auto point_num = points.size();
            if (point_num > 1) {
                x_step = points[point_num-1].x() - points[point_num-2].x();
            }
            point = QPointF(x_end + x_step, value);
        }
        emit receiveCurvePoint(point);
    }

};
