/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : AppMonitor.h
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREUI_APPMONITOR_H
#define FIREUI_APPMONITOR_H

#include "TSingleton.h"


namespace TF {

    class TFRuntimeException;

    class AppMonitor : public TBase::TSingleton<AppMonitor>
    {
    public:
        static int initApp(int argc, char *argv[]);

    private:
        static void initAppLog(int argc, char *argv[]);

        static void exitApp(TFRuntimeException& ex);

    };

} // TF

#endif //FIREUI_APPMONITOR_H