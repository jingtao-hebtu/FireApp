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


namespace TF {

    class FuSideTabBar;
    class FuMainMeaPage;
    class FuCamPage;

    class FuMainWid_Ui {

        friend class FuMainWid;

    public:

        void setupUi(QWidget* wid);

    protected:

        QWidget *mWid {nullptr};
        QHBoxLayout *mLayout {nullptr};

        QWidget *mContentWidget {nullptr};
        QVBoxLayout *mContentLayout {nullptr};

        FuSideTabBar *mSideTabBar {nullptr};

        QVector<QWidget *> mPages;

        FuMainMeaPage *mMainMeaPage {nullptr};
        FuCamPage *mCamPage {nullptr};
    };

};




#endif //EPVIDEOVIEWER_FUMAINWID_UI_H
