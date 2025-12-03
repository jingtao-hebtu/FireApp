/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TParamHandler.h
   Author : tao.jing
   Date   : 2023/12/10
   Brief  : 
**************************************************************************/
#ifndef BREAKERMEA_TPARAMHANDLER_H
#define BREAKERMEA_TPARAMHANDLER_H

#include "TParam.h"
#include <string>
#include <memory>
#include <map>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif


namespace TBase {

    class TAO_UTIL_API TParamHandler {

    public:
        TParamHandler();

        virtual ~TParamHandler();

        TParam &operator[](std::string const &);

        TParam &operator[](const char[]);

    private:
        class Impl;
        std::unique_ptr<Impl> mImpl;

    };

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //BREAKERMEA_TPARAMHANDLER_H
