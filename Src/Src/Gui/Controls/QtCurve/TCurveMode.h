/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : CurveMode.h
   Author : tao.jing
   Date   : 2023/11/29
   Brief  : 
**************************************************************************/
#ifndef QONEDARK_CURVEMODE_H
#define QONEDARK_CURVEMODE_H


namespace T_QtBase {

    enum CurveXAxisMode {
        XAxis_Fixed = 0,
        XAxis_Discrete = 1,
        XAxis_Continuous = 2,
    };

    enum CurveYAxisMode {
        YAxis_Fixed = 0,
        YAxis_Adaptive = 1
    };

    enum CurveXUpdateMode {
        XUpdate_Scroll = 0,
        XUpdate_Sweep = 1,
    };

    enum CurveSeriesMode {
        Series_LineSeries = 0,
        Series_SplineSeries = 1
    };

    struct CurveMode {
        CurveXAxisMode mXAxisMode {XAxis_Continuous};
        CurveYAxisMode mYAxisMode {YAxis_Adaptive};
        CurveXUpdateMode mXUpdateMode {XUpdate_Scroll};
        CurveSeriesMode mSeriesMode {Series_LineSeries};
    };

};

#endif //QONEDARK_CURVEMODE_H
