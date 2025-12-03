/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TJsonHandler.h
   Author : tao.jing
   Date   : 2024/7/25
   Brief  :
**************************************************************************/
#ifndef TUTILS_TJSONHANDLER_H
#define TUTILS_TJSONHANDLER_H

#include <string>
#include <vector>
#include <memory>
#include "TaoUtilDefine.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif


namespace TBase {

    class TAO_UTIL_API TJsonHandler {

    public:
        TJsonHandler();

        ~TJsonHandler();

        explicit TJsonHandler(const char *file_name);

        explicit TJsonHandler(const std::string &file_name);

        void load(const std::string &file_name);

        void fetchKeys(std::vector<std::string> &keys);

        void showContents();

        template<typename T>
        T getValue(std::initializer_list<const std::string> keys);

        template<typename T>
        T getArrayValue(std::initializer_list<const std::string> keys, std::size_t index);

    private:
        class Impl;
        std::unique_ptr<Impl> mImpl;
    };

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif


#endif //TUTILS_TJSONHANDLER_H
