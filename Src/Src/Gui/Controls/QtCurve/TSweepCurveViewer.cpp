/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TSweepCurveViewer.cpp
   Author : tao.jing
   Date   : 2023/11/29
   Brief  : 
**************************************************************************/
#include "TSweepCurveViewer.h"
#include "TCurveViewer_Ui.h"
#include "TException.h"
#include "TLog.h"
#include <QXYSeries>
#include <QValueAxis>


namespace T_QtBase {

    TSweepCurveViewer::TSweepCurveViewer() {
        mUi = new TCurveViewer_Ui();
        mUi->setupUi(this);

        initAction();
    }

    void TSweepCurveViewer::initAction() {
        connect(this, SIGNAL(receiveIndexValue(std::int32_t,float)),
                this, SLOT(onReceiveIndexValue(std::int32_t,float)));
        connect(this, SIGNAL(receiveIndexValues(const std::vector<std::int32_t>&,const std::vector<float>&)),
                this, SLOT(onReceiveIndexValues(const std::vector<std::int32_t>&,const std::vector<float>&)));        connect(this, SIGNAL(receiveNextValue(float)),
                this, SLOT(onReceiveNextValue(float)));
    }

    void TSweepCurveViewer::updateXAxisRange(std::int32_t x_min, std::int32_t x_max) {
        mXMin = x_min;
        mXMax = x_max;
        mUi->mAxisX->setRange(x_min, x_max);
    }

    void TSweepCurveViewer::updateYAxisRange(std::int32_t y_min, std::int32_t y_max) {
        mYMin = y_min;
        mYMax = y_max;
        mUi->mAxisY->setRange(y_min, y_max);
    }

    void TSweepCurveViewer::onReceiveIndexValue(std::int32_t index, float value) {
        // Validate index
        validateIndex(index);

        // Update the mUpdateSeries
        QPointF start_point(index, getYMin());
        QPointF end_point(index, getYMax());

        auto update_series_point_num = mUi->mUpdateSeries->points().size();
        if (update_series_point_num == 0) {
            mUi->mUpdateSeries->append(QList<QPointF>{start_point, end_point});
        } else if (update_series_point_num == 2) {
            mUi->mUpdateSeries->replace(0, start_point);
            mUi->mUpdateSeries->replace(1, end_point);
        } else {
            TBASE_LOG_THROW_RUNTIME("SweepCurveViewer: Invalid update series point num:  %d.", update_series_point_num);
        }

        // Update the mCurveSeries
        auto curve_series_point_num = mUi->mCurveSeries->points().size();
        if (curve_series_point_num <= index) {
            auto padding_len = index - curve_series_point_num;
            for (auto padding_idx = 0; padding_idx < padding_len; padding_idx++) {
                mUi->mCurveSeries->append(padding_idx, 0);
            }
            mUi->mCurveSeries->append(index, value);
        } else {
            mUi->mCurveSeries->replace(index, QPointF(index, value));
        }
        mLastUpdateIndex = index;
    }

    void TSweepCurveViewer::onReceiveIndexValues(const std::vector<std::int32_t>& indexes,
                                                const std::vector<float>& values) {
        if (indexes.size() != values.size()) {
            TBASE_LOG_THROW_RUNTIME("[SweepCurveViewer::onReceiveIndexValues] indexes and values not with same length.");
        }

        auto start_index = indexes.front();
        auto end_index = indexes.back();
        auto indexes_size = indexes.size();

        if (end_index - start_index + 1 == indexes_size) {
            // --> Ordered indexes
            validateIndex(start_index);
            validateIndex(end_index);

            // Padding curve series
            auto curve_series_point_num = mUi->mCurveSeries->points().size();
            auto padding_len = end_index - curve_series_point_num;
            for (auto padding_idx = 0; padding_idx < padding_len; padding_idx++) {
                mUi->mCurveSeries->append(padding_idx, 0);
            }

            QList<QPointF> points;
            for (auto idx = 0; idx < indexes.size(); idx++) {
                points.append(QPointF(indexes[idx], values[idx]));
            }
            mUi->mCurveSeries->replace(points);
            mLastUpdateIndex = indexes.back();
        } else {
            // --> Not ordered indexes
            for (auto idx = 0; idx < indexes.size(); idx++) {
                onReceiveIndexValue(indexes[idx], values[idx]);
            }
        }
    }

    void TSweepCurveViewer::onReceiveNextValue(float value) {
        mLastUpdateIndex ++;
        if (mLastUpdateIndex > getXMax()) {
            mLastUpdateIndex = getXMin();
        }
        onReceiveIndexValue(mLastUpdateIndex, value);
    }

    void TSweepCurveViewer::validateIndex(std::int32_t index) {
        auto x_min = getXMin();
        auto x_max = getXMax();
        if (index < x_min || index > x_max) {
            TBASE_LOG_THROW_RUNTIME("Invalid point index %d, for x_min %d and x_max %d.", index, x_min, x_max);
        }
    }


};

