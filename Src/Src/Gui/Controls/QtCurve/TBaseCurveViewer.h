/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TBaseCurveViewer.h
   Author : tao.jing
   Date   : 2023/11/28
   Brief  : 
**************************************************************************/
#ifndef QONEDARK_TBASECURVEVIEWER_H
#define QONEDARK_TBASECURVEVIEWER_H

#include "TCurveMode.h"
#include <cstdint>
#include <QChartView>
#include "TaoUtilDefine.h"


namespace T_QtBase {

    class TBaseCurveViewer : public QChartView {

        Q_OBJECT
    public:
        TBaseCurveViewer() = default;
        TBaseCurveViewer(std::int32_t x_min,
                         std::int32_t x_max,
                         std::int32_t y_min,
                         std::int32_t y_max,
                         CurveXAxisMode x_axis_mode = XAxis_Continuous,
                         CurveYAxisMode y_axis_mode = YAxis_Adaptive,
                         CurveXUpdateMode x_update_mode = XUpdate_Scroll,
                         CurveSeriesMode series_mode = Series_LineSeries);


        [[nodiscard]] CurveSeriesMode getSeriesMode() const {return mCurveMode.mSeriesMode;}
        [[nodiscard]] QColor getCurveColor() const {return mCurveColor;}
        [[nodiscard]] std::int32_t getXMin() const {return mXMin;}
        [[nodiscard]] std::int32_t getXMax() const {return mXMax;}
        [[nodiscard]] std::int32_t getYMin() const {return mYMin;}
        [[nodiscard]] std::int32_t getYMax() const {return mYMax;}

        virtual void updateXAxisRange(std::int32_t x_min, std::int32_t x_max) = 0;
        virtual void updateYAxisRange(std::int32_t y_min, std::int32_t y_max) = 0;

    protected:
        std::int32_t mXMin {0};
        std::int32_t mXMax {255};
        std::int32_t mYMin {0};
        std::int32_t mYMax {255};

        CurveMode mCurveMode;
        QColor mCurveColor {QColor(0x00FF00)};
    };

};


#endif //QONEDARK_BASECURVEVIEWER_H
