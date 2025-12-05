/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TBaseCurveViewer.cpp
   Author : tao.jing
   Date   : 2023/11/28
   Brief  : 
**************************************************************************/
#include "TBaseCurveViewer.h"


namespace T_QtBase {

    TBaseCurveViewer::TBaseCurveViewer(std::int32_t x_min,
                                       std::int32_t x_max,
                                       std::int32_t y_min,
                                       std::int32_t y_max,
                                       CurveXAxisMode x_axis_mode,
                                       CurveYAxisMode y_axis_mode,
                                       CurveXUpdateMode x_update_mode,
                                       CurveSeriesMode series_mode)
            : mXMin(x_min), mXMax(x_max), mYMin(y_min), mYMax(y_max) {

        mCurveMode.mXAxisMode = x_axis_mode;
        mCurveMode.mYAxisMode = y_axis_mode;
        mCurveMode.mXUpdateMode = x_update_mode;
        mCurveMode.mSeriesMode = series_mode;

    }


};
