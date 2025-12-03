/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TIniHelper.h
   Author : tao.jing
   Date   : 2023/12/12
   Brief  :
**************************************************************************/
#ifndef BREAKERMEA_TINIHANDLER_H
#define BREAKERMEA_TINIHANDLER_H

#include <string>
#include <memory>
#include <vector>
#include "TaoUtilDefine.h"


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace TBase {

    class TAO_UTIL_API TIniHandler {

    public:
        TIniHandler();

        ~TIniHandler();

        explicit TIniHandler(const char *file_name);

        explicit TIniHandler(const std::string &file_name);

        void load(const std::string &file_name);

        void fetchKeys(std::vector<std::string> &keys);

        void showContents();

        template<typename T>
        T getValue(std::initializer_list<const std::string> keys);

    private:
        class Impl;
        std::unique_ptr<Impl> mImpl;
    };

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //BREAKERMEA_TINIHANDLER_H
