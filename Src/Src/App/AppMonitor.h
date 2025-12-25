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

    class FuMainWid;

    class TFRuntimeException;

    class AppMonitor : public TBase::TSingleton<AppMonitor>
    {
    public:
        static int initApp(int argc, char *argv[]);

        void initAfterWid();

        void setMainWid(FuMainWid *wid) {mMainWid = wid;};

        void promptError(const char* msg);

    private:
        static void initAppLog(int argc, char *argv[]);

        static void exitApp(TFRuntimeException& ex);

    private:
        FuMainWid *mMainWid{nullptr};

    };

} // TF

#endif //FIREUI_APPMONITOR_H