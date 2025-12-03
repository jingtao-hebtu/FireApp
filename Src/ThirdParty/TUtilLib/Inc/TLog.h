/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TLog.h
   Author : tao.jing
   Date   : 2023/11/26
   Brief  : 
**************************************************************************/

#ifndef BREAKERMEA_TLOG_H
#define BREAKERMEA_TLOG_H


#include "TaoUtilDefine.h"

#ifdef LOGURU_EXPORT
#undef LOGURU_EXPORT
#endif

#define LOGURU_EXPORT TAO_UTIL_API
#include "loguru.hpp"


namespace TBase {

    void TAO_UTIL_API initDateLog(int argc, char *argv[]);

};


#endif //BREAKERMEA_TLOG_H
