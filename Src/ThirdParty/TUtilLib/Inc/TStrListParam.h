/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TStrListParam.h
   Author : tao.jing
   Date   : 2024/8/9
   Brief  :
**************************************************************************/
#ifndef TUTILS_TSTRLISTPARAM_H
#define TUTILS_TSTRLISTPARAM_H

#include <map>
#include <vector>
#include <string>
#include "TaoUtilDefine.h"


namespace TBase {


    class TAO_UTIL_API TStrListParam {

    public:
        TStrListParam() = default;

        TStrListParam(const std::map<std::string, std::string> &params);

        TStrListParam(std::initializer_list<std::pair<std::string, std::string>> params);

        std::string &operator[](const std::string &key);

        auto begin() { return mStrParamMap.begin(); }

        auto end() { return mStrParamMap.end(); }

        [[nodiscard]] auto begin() const { return mStrParamMap.begin(); }

        [[nodiscard]] auto end() const { return mStrParamMap.end(); }

    private:
        std::map<std::string, std::string> mStrParamMap;
    };

};


#endif //TUTILS_TSTRLISTPARAM_H
