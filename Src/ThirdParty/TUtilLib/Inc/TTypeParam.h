/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TTypeParam.h
   Author : tao.jing
   Date   : 2024/5/29
   Brief  :
**************************************************************************/
#ifndef TUTILS_TTYPEPARAM_H
#define TUTILS_TTYPEPARAM_H

#include <typeindex>
#include <stdexcept>
#include <string>
#include "TaoUtilDefine.h"


namespace TBase {

    class TAO_UTIL_API TTypeParamBase {
    public:
        virtual ~TTypeParamBase() = default;

        [[nodiscard]] virtual std::type_index getType() const = 0;

        virtual void setValue(const TTypeParamBase &value) = 0;

        virtual std::string getValueAsString() const = 0;
    };


    template<typename T>
    class TTypeParam : public TTypeParamBase {

    public:
        explicit TTypeParam(T value) : value(value) {}

        T getValue() const { return value; }

        [[nodiscard]] std::type_index getType() const override { return typeid(T); }

        void setValue(const TTypeParamBase &newValue) override;

        void setValue(T newValue);

        [[nodiscard]] std::string getValueAsString() const override;

    private:
        T value;
    };


};



#endif //TUTILS_TTYPEPARAM_H
