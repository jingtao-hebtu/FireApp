/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuFlameCurvePage.h
   Author : tao.jing
   Date   : 2026/03/02
   Brief  : 检测曲线显示页面，显示火焰高度、面积和HRR的实时曲线
**************************************************************************/
#ifndef FIREAPP_FUFLAMECURVEPAGE_H
#define FIREAPP_FUFLAMECURVEPAGE_H

#include <QWidget>

class QVBoxLayout;

namespace T_QtBase {
    class TSweepCurveViewer;
}

namespace TF {

    class FuFlameCurvePage : public QWidget {
        //Q_OBJECT

    public:
        explicit FuFlameCurvePage(QWidget *parent = nullptr);
        ~FuFlameCurvePage() override = default;

    private:
        void setupUi();
        T_QtBase::TSweepCurveViewer* createCurveViewer(int x_min, int x_max, int y_min, int y_max);
        void addCurveRow(QVBoxLayout *layout, const QString &label, T_QtBase::TSweepCurveViewer *viewer);

    private:
        T_QtBase::TSweepCurveViewer *mHeightCurveViewer {nullptr};
        T_QtBase::TSweepCurveViewer *mAreaCurveViewer {nullptr};
        T_QtBase::TSweepCurveViewer *mHrrCurveViewer {nullptr};
    };

} // TF

#endif //FIREAPP_FUFLAMECURVEPAGE_H
