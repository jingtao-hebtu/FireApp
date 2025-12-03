/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : AppLocalConfig.h
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#ifndef FIREUI_APPLOCALCONFIG_H
#define FIREUI_APPLOCALCONFIG_H

#include "TSingleton.h"


namespace TF {
    class AppLocalConfig : public TBase::TSingleton<AppLocalConfig>
    {
    public:
        static void initConfigs();

    private:
        static void initAppConfig();
    };
} // TF

#endif //FIREUI_APPLOCALCONFIG_H
