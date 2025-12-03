/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TStrParamHandler.h
   Author : tao.jing
   Date   : 2024/6/2
   Brief  :
**************************************************************************/
#ifndef TUTILS_TSTRPARAMHANDLER_H
#define TUTILS_TSTRPARAMHANDLER_H

#include "TStrListParam.h"
#include <map>
#include <string>
#include <memory>
#include "TaoUtilDefine.h"


namespace TBase {

    class TAO_UTIL_API TStrParamHandler {

    public:

        TStrParamHandler();

        ~TStrParamHandler();

        TStrParamHandler(std::initializer_list<std::pair<std::string, std::string>> args);

        TStrParamHandler(const TStrListParam& args);

        template<typename T>
        T get(const std::string& name) const;

        template<typename T>
        void set(const std::string& name, const T& value);

        [[nodiscard]] const std::map<std::string, std::string>& getParams() const;

        void merge(const TStrParamHandler& other);

        void merge(const std::map<std::string, std::string> &params);

    private:

        template<typename T>
        T convertTo(const std::string& name) const;

    private:
        class Impl;
        std::unique_ptr<Impl> mImpl;
        //std::map <std::string, std::string> mParams;
    };

    template<>
    TAO_UTIL_API bool TStrParamHandler::convertTo<bool>(const std::string& name) const;

};


#endif //TUTILS_TSTRPARAMHANDLER_H
