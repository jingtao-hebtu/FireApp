/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TFException.h
   Author : tao.jing
   Date   : 2025/11/12
   Brief  :
**************************************************************************/
#ifndef FIREUI_TFEXCEPTION_H
#define FIREUI_TFEXCEPTION_H

#include "TLog.h"
#include <cstdio>
#include <cstdarg>
#include <exception>
#include <string>


#define TF_LOG_THROW_RUNTIME(...) \
{                              \
LOG_F(ERROR, __VA_ARGS__);     \
throw TF::TFRuntimeException(TF::formatVarsToString(__VA_ARGS__)); \
}

#define TF_LOG_THROW_PROMPT(...) \
{                              \
LOG_F(ERROR, __VA_ARGS__);     \
throw TF::TFPromptException(TF::formatVarsToString(__VA_ARGS__)); \
}


namespace TF {
    static inline std::string formatVarsToString(const char* fmt, ...) {
        char buf[512] = {0};
        std::va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        return buf;
    }

    class TFException : public std::exception
    {
    public:
        explicit TFException(const char* msg) : message_(msg) {
        }

        explicit TFException(const std::string& msg) : message_(msg) {
        }

        [[nodiscard]] const char* what() const noexcept override {
            return message_.c_str();
        }

    private:
        std::string message_;
    };

    // Prompt exception information to user
    class TFPromptException : public TFException
    {
    public:
        explicit TFPromptException(const char* msg) : TFException(msg) {
        }

        explicit TFPromptException(const std::string& msg) : TFException(msg) {
        }
    };

    // Terminate the program
    class TFRuntimeException : public TFException
    {
    public:
        explicit TFRuntimeException(const char* msg) : TFException(msg) {
        }

        explicit TFRuntimeException(const std::string& msg) : TFException(msg) {
        }
    };
}

#endif //FIREUI_TFEXCEPTION_H
