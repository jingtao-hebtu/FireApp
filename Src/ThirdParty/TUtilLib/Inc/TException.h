/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TException.h
   Author : tao.jing
   Date   : 2023/12/2
   Brief  : 
**************************************************************************/
#ifndef BREAKERMEA_TEXCEPTION_H
#define BREAKERMEA_TEXCEPTION_H

#include "TLog.h"
#include <cstdarg>
#include <cstdio>
#include <stdexcept>
#include <string>


#define TBASE_LOG_THROW_RUNTIME(...) \
{                              \
LOG_F(ERROR, __VA_ARGS__);     \
throw TBase::TBaseException(TBase::formatVarsToString(__VA_ARGS__)); \
}


namespace TBase {

    static inline std::string formatVarsToString(const char *fmt, ...) {
        char buf[512] = {0};
        std::va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        return std::string(buf);
    }

    class TBaseException : public std::runtime_error {

    public:
        explicit TBaseException(const char *msg) : std::runtime_error(msg ? msg : "") {}

        explicit TBaseException(const std::string &msg) : std::runtime_error(msg) {}

    };

};


#endif //BREAKERMEA_TEXCEPTION_H
