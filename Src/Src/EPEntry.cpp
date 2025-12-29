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
#include <QScreen>
#include <QtGlobal>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    const auto args = a.arguments();
    const bool dumpLayout = args.contains("--dump-layout") || qEnvironmentVariableIsSet("UI_DUMP_LAYOUT");

    TF::AppMonitor::initApp(argc, argv);

    // Image detection test
    //extern void detectTest();
    //detectTest();

    TF::FuMainWid win;
    win.show();
    TF::AppMonitor::instance().setMainWid(&win);

    auto scr = QGuiApplication::primaryScreen();
    qInfo() << "availableGeometry" << scr->availableGeometry();
    if (scr->availableGeometry().width() < 1100) {
        win.showFullScreen();
    }

    //frmConfigIpcSearch win;
    //win.show();

    TF::AppMonitor::instance().initAfterWid();

    if (dumpLayout) {
        a.processEvents();
        win.dumpLayoutDiagnostics();
        return 0;
    }

    return QApplication::exec();
}