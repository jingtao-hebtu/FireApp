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


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    TF::AppMonitor::initApp(argc, argv);

    //extern void detectTest();
    //detectTest();

    TF::FuMainWid win;
    win.show();

    return QApplication::exec();
}