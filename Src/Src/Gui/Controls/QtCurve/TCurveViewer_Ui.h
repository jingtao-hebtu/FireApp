/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TCurveViewer_Ui.h
   Author : tao.jing
   Date   : 2023/11/29
   Brief  : 
**************************************************************************/
#ifndef QONEDARK_TCURVEVIEWER_UI_H
#define QONEDARK_TCURVEVIEWER_UI_H

#include "TCurveMode.h"
#include <string>


class QChart;
class QValueAxis;
class QXYSeries;


namespace T_QtBase {

    class TBaseCurveViewer;
    class TScrollCurveViewer;
    class TSweepCurveViewer;
    class TPlotCurveViewer;

    class TCurveViewer_Ui {

        friend class TBaseCurveViewer;
        friend class TScrollCurveViewer;
        friend class TSweepCurveViewer;
        friend class TPlotCurveViewer;

    public:
        TCurveViewer_Ui() = default;
        explicit TCurveViewer_Ui(CurveXUpdateMode x_update_mode);

        void setupUi(TBaseCurveViewer* viewer);
        void switchColorScheme(const std::string& scheme_name);

    protected:
        bool mUseOpenGL {false};
        CurveXUpdateMode mXUpdateMode {XUpdate_Sweep};

        QChart* mChart {nullptr};
        QValueAxis* mAxisX {nullptr};
        QValueAxis* mAxisY {nullptr};
        QXYSeries* mCurveSeries {nullptr};
        QXYSeries* mUpdateSeries {nullptr};
    };
};



#endif //QONEDARK_CURVEVIEWER_UI_H
