/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TSweepCurveViewer.h
   Author : tao.jing
   Date   : 2023/11/29
   Brief  : 
**************************************************************************/
#ifndef QONEDARK_SWEEPCURVEVIEWER_H
#define QONEDARK_SWEEPCURVEVIEWER_H

#include "TBaseCurveViewer.h"
#include <cstdint>
#include <vector>
#include "TaoUtilDefine.h"


namespace T_QtBase {

    class TCurveViewer_Ui;

    class TSweepCurveViewer : public TBaseCurveViewer {

    Q_OBJECT
    public:
        explicit TSweepCurveViewer();

        void updateXAxisRange(std::int32_t x_min, std::int32_t x_max) final;
        void updateYAxisRange(std::int32_t y_min, std::int32_t y_max) final;

    signals:

        void receiveIndexValue(std::int32_t index, float value);
        void receiveIndexValues(const std::vector<std::int32_t> &indexes,
                                const std::vector<float> &values);
        void receiveNextValue(float);

    public slots:

        void onReceiveIndexValue(std::int32_t index, float value);
        void onReceiveIndexValues(const std::vector<std::int32_t> &indexes,
                                  const std::vector<float> &values);
        void onReceiveNextValue(float value);

    private:
        void initAction();

        void validateIndex(std::int32_t index);

    private:
        TCurveViewer_Ui *mUi{nullptr};
        std::int32_t mLastUpdateIndex {0};
    };

};


#endif //QONEDARK_SWEEPCURVEVIEWER_H
