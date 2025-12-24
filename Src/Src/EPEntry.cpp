/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : EPEntry.cpp
   Author : tao.jing
   Date   : 2025/1/9
   Brief  :
**************************************************************************/
#include <QApplication>
#include "AppMonitor.h"
#include "FuMainWid.h"
#include "frmconfigipcsearch.h"


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    TF::AppMonitor::initApp(argc, argv);

    // Image detection test
    //extern void detectTest();
    //detectTest();

    TF::FuMainWid win;
    win.show();

    //frmConfigIpcSearch win;
    //win.show();

    TF::AppMonitor::instance().initAfterWid();

    return QApplication::exec();
}