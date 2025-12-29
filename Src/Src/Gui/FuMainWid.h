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

#include "HKCamPythonServer.h"

namespace TF {

    class FuMainWid_Ui;

    class TFDistClient;

    class FuMainWid : public QWidget {

        Q_OBJECT

    public:
        explicit FuMainWid(QWidget *parent = nullptr);

        ~FuMainWid() final;

        void initAfterDisplay();

        void dumpLayoutDiagnostics() const;

    private:
        void setupUi();
        void setupConnections();

        void initStyle();

        void initActions();

        void startHKCamPythonServer();
        void stopHKCamPythonServer();

    signals:
        void promptError(const QString &msg);

    private slots:
        void onPromptError(const QString &msg);

    private:
        FuMainWid_Ui *mUi;
        HKCamPythonServer mCamPythonServer;

    };

};


#endif //EPVIDEOVIEWER_FUMAINWID_H
