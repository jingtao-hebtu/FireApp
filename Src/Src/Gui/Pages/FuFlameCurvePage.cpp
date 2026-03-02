/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuFlameCurvePage.cpp
   Author : tao.jing
   Date   : 2026/03/02
   Brief  : 检测曲线显示页面，显示火焰高度、面积和HRR的实时曲线
**************************************************************************/
#include "FuFlameCurvePage.h"
#include "TSweepCurveViewer.h"
#include "TFMeaManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QSizePolicy>
#include <QCoreApplication>

namespace TF {

    FuFlameCurvePage::FuFlameCurvePage(QWidget *parent) : QWidget(parent) {
        setupUi();
    }

    T_QtBase::TSweepCurveViewer* FuFlameCurvePage::createCurveViewer(
            int x_min, int x_max, int y_min, int y_max) {
        auto *viewer = new T_QtBase::TSweepCurveViewer();
        viewer->updateXAxisRange(x_min, x_max);
        viewer->updateYAxisRange(y_min, y_max);
        return viewer;
    }

    void FuFlameCurvePage::addCurveRow(QVBoxLayout *layout, const QString &label,
                                       T_QtBase::TSweepCurveViewer *viewer) {
        auto *row = new QWidget(this);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(8);

        auto *nameLabel = new QLabel(label, row);
        nameLabel->setObjectName("CurveNameLabel");
        nameLabel->setFixedWidth(56);
        nameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        nameLabel->setWordWrap(true);

        rowLayout->addWidget(nameLabel);
        rowLayout->addWidget(viewer, 1);

        layout->addWidget(row, 1);
    }

    void FuFlameCurvePage::setupUi() {
        setObjectName("FlameCurvePage");

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(12, 12, 12, 12);
        mainLayout->setSpacing(8);

        auto *titleLabel = new QLabel(QCoreApplication::translate("Page", "检测曲线"), this);
        titleLabel->setObjectName("PanelTitle");
        titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        auto *frame = new QFrame(this);
        frame->setObjectName("StatisticsFrame");
        auto *frameLayout = new QVBoxLayout(frame);
        frameLayout->setContentsMargins(12, 12, 12, 12);
        frameLayout->setSpacing(8);

        mHeightCurveViewer = createCurveViewer(0, 100, 0, 6);
        mHeightCurveViewer->setObjectName("FlameCurveHeight");

        mAreaCurveViewer = createCurveViewer(0, 100, 0, 6);
        mAreaCurveViewer->setObjectName("FlameCurveArea");

        mHrrCurveViewer = createCurveViewer(0, 100, 0, 100);
        mHrrCurveViewer->setObjectName("FlameCurveHRR");

        addCurveRow(frameLayout, QCoreApplication::translate("Page", "火焰\n高度"), mHeightCurveViewer);
        addCurveRow(frameLayout, QCoreApplication::translate("Page", "火焰\n面积"), mAreaCurveViewer);
        addCurveRow(frameLayout, QCoreApplication::translate("Page", "HRR"), mHrrCurveViewer);

        mainLayout->addWidget(titleLabel);
        mainLayout->addWidget(frame, 1);

        TFMeaManager::instance().setFlameCurveHeightViewer(mHeightCurveViewer);
        TFMeaManager::instance().setFlameCurveAreaViewer(mAreaCurveViewer);
        TFMeaManager::instance().setFlameCurveHrrViewer(mHrrCurveViewer);
    }

} // TF
