/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuMainWid_Ui.h
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREUI_FUMAINWID_UI_H
#define FIREUI_FUMAINWID_UI_H

#include <QHBoxLayout>
#include <QVector>
#include <QStackedLayout>


namespace TF {

    class FuSideTabBar;
    class FuMainMeaPage;
    class FuCamPage;
    class ExperimentDataViewPage;

    class FuMainWid_Ui {

        friend class FuMainWid;

    public:

        void setupUi(QWidget* wid);

    protected:

        QWidget *mWid {nullptr};
        QHBoxLayout *mLayout {nullptr};

        QWidget *mContentWidget {nullptr};
        QStackedLayout *mContentLayout {nullptr};

        FuSideTabBar *mSideTabBar {nullptr};

        QVector<QWidget *> mPages;

        FuMainMeaPage *mMainMeaPage {nullptr};
        FuCamPage *mCamPage {nullptr};
        ExperimentDataViewPage *mExperimentViewPage {nullptr};
    };

};




#endif //EPVIDEOVIEWER_FUMAINWID_UI_H
