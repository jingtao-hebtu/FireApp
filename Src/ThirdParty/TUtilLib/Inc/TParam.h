/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TParam.h
   Author : tao.jing
   Date   : 2023/12/11
   Brief  :
**************************************************************************/
#ifndef BREAKERMEA_TPARAM_H
#define BREAKERMEA_TPARAM_H

#include <string>
#include <map>
#include <sstream>
#include "TaoUtilDefine.h"


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif


namespace TBase {

    class TAO_UTIL_API TParam {

    public:
        enum Status {
            MAP, VALUE
        };

    public:
        TParam() = default;

        explicit TParam(std::string value);

        explicit TParam(std::map<std::string, TParam> const &map);

        template<class T>
        TParam &operator=(T value);

        template<class T>
        operator T() const {
            std::stringstream ss(mValue);
            T num;
            ss >> num;
            return num;
        }

        TParam &operator[](std::string const &);

        TParam &operator[](const char[]);

    private:
        TParam::Status mStatus{VALUE};
        std::string mValue;
        std::map<std::string, TParam> mParamMap;
    };

    extern template TAO_UTIL_API TParam &TParam::operator=<bool>(bool);

    extern template TAO_UTIL_API TParam &TParam::operator=<int>(int);

    extern template TAO_UTIL_API TParam &TParam::operator=<double>(double);

    extern template TAO_UTIL_API TParam &TParam::operator=<std::string>(std::string);

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //BREAKERMEA_TPARAM_H
