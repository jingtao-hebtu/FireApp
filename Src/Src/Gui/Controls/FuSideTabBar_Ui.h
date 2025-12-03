/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuSideTabBar_Ui.h
   Author : OpenAI ChatGPT
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREUI_FUSIDETABBAR_UI_H
#define FIREUI_FUSIDETABBAR_UI_H

#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QList>
#include <QVector>

namespace TF {

    class FuSideTabBar_Ui {
        friend class FuSideTabBar;
    public:
        void setupUi(QWidget* wid);

    protected:
        QWidget *mWid {nullptr};
        QVBoxLayout *mLayout {nullptr};
        QList<QToolButton*> mButtons {};
    };
}

#endif //FIREUI_FUSIDETABBAR_UI_H
