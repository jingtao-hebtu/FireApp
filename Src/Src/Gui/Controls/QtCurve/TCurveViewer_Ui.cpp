/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TCurveViewer_Ui.cpp
   Author : tao.jing
   Date   : 2023/11/29
   Brief  :
**************************************************************************/
#include "TCurveViewer_Ui.h"
#include "TBaseCurveViewer.h"
#include "TCurveColor.h"
#include <QChart>
#include <QValueAxis>
#include <QLineSeries>
#include <QSplineSeries>
#include <QGraphicsLayout>



namespace T_QtBase {

    TCurveViewer_Ui::TCurveViewer_Ui(CurveXUpdateMode x_update_mode) : mXUpdateMode (x_update_mode) {
    }

    void TCurveViewer_Ui::setupUi(TBaseCurveViewer *viewer) {
        viewer->setContentsMargins(0, 0, 0, 0);
        mChart = new QChart();

        mAxisX = new QValueAxis(viewer);
        mAxisY = new QValueAxis(viewer);

        if (viewer->getSeriesMode() == Series_LineSeries) {
            mCurveSeries = new QLineSeries(viewer);
        } else if (viewer->getSeriesMode() == Series_SplineSeries) {
            mCurveSeries = new QSplineSeries(viewer);
        }
        mCurveSeries->setUseOpenGL(mUseOpenGL);

        viewer->setChart(mChart);
        mChart->addSeries(mCurveSeries);
        mChart->addAxis(mAxisX, Qt::AlignBottom);
        mChart->addAxis(mAxisY, Qt::AlignLeft);
        mCurveSeries->attachAxis(mAxisX);
        mCurveSeries->attachAxis(mAxisY);
        mChart->legend()->hide();
        mChart->setMargins(QMargins(0, 0, 0, 0));
        mChart->layout()->setContentsMargins(0, 0, 0, 0);
        mChart->setBackgroundRoundness(0);
        mChart->setBackgroundBrush(QBrush(QColor(0, 0, 0)));

        QPen pen(QColor(viewer->getCurveColor()));
        pen.setWidth(1);
        mCurveSeries->setPen(pen);

        if (mXUpdateMode == XUpdate_Sweep) {
            mUpdateSeries = new QLineSeries(viewer);
            mUpdateSeries->setUseOpenGL(mUseOpenGL);
            mChart->addSeries(mUpdateSeries);
            mUpdateSeries->attachAxis(mAxisX);
            mUpdateSeries->attachAxis(mAxisY);

            QPen pen_update(QColor("purple"));
            pen_update.setWidth(2);
            mUpdateSeries->setPen(pen_update);
        }

        mAxisX->setTickCount(20);
        mAxisX->setRange(viewer->getXMin(), viewer->getXMax());
        mAxisY->setRange(viewer->getYMin(), viewer->getYMax());
        mAxisX->hide();
        mAxisY->hide();
    }

    void TCurveViewer_Ui::switchColorScheme(const std::string& scheme_name) {
        QColor color;
        auto err_no = TCurveColor::getSchemeColorByName(scheme_name, "CurveViewBkColor", color);
        if (err_no == TBase::E_T_SUCCESS) {
            mChart->setBackgroundBrush(QBrush(color));
        }
    }
};