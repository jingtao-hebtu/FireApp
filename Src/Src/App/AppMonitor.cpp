/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : AppMonitor.cpp
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "AppMonitor.h"
#include <iostream>
#include "TException.h"
#include "TFException.h"
#include "TFError.h"
#include "AppLocalConfig.h"
#include "TLog.h"


int TF::AppMonitor::initApp(int argc, char* argv[]) {

    try {
        initAppLog(argc, argv);
    } catch (std::exception &ex) {
        std::cerr << "Init app log failed: " << ex.what() <<std::endl;
    }

    try {
        AppLocalConfig::instance().initConfigs();
    } catch (TFRuntimeException& ex) {
        exitApp(ex);
    } catch (std::exception &ex) {
        LOG_F(ERROR, "Init app failed %s.", ex.what());
    }
    return 0;
}

void TF::AppMonitor::initAppLog(int argc, char* argv[]) {
    try {
        TBase::initDateLog(argc, argv);
    } catch (TBase::TBaseException&) {
        throw TFRuntimeException("Init app log failed. (AppMonitor::initAppLog)");
    }
}

void TF::AppMonitor::exitApp(TFRuntimeException& ex) {
    std::cerr << "----- Fatal error: " << std::endl;
    std::cerr << ex.what() << std::endl;
    std::cerr << "----- Terminate." << std::endl;
    exit(TF::TFError::T_ERROR);
}