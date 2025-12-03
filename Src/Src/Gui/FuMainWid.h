/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FuMainWid.h
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREUI_FUMAINWID_H
#define FIREUI_FUMAINWID_H

#include <QWidget>


namespace TF {

    class FuMainWid_Ui;

    class FuMainWid : public QWidget {

        Q_OBJECT

    public:
        explicit FuMainWid(QWidget *parent = nullptr);

        ~FuMainWid() final;

    private:
        void setupUi();
        void setupConnections();

        void initStyle();

    private:
        FuMainWid_Ui *mUi;

    };

};


#endif //EPVIDEOVIEWER_FUMAINWID_H
