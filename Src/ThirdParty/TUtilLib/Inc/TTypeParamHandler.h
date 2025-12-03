/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TTypeParamHandler.h
   Author : tao.jing
   Date   : 2024/5/29
   Brief  :
**************************************************************************/
#ifndef TUTILS_TTYPEPARAMHANDLER_H
#define TUTILS_TTYPEPARAMHANDLER_H

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <utility>
#include <variant>
#include <map>
#include "TTypeParam.h"
#include "TaoUtilDefine.h"


namespace TBase {

    using TInitParamTypes = std::variant<bool, int, double, std::string>;

    class TAO_UTIL_API TTypeParamHandler {

    public:
        class ParamAccessor {
        public:
            ParamAccessor(std::shared_ptr<TTypeParamBase> param) : param(std::move(param)) {}

            template<typename T>
            operator T() const {
                if (param->getType() == typeid(T)) {
                    return std::static_pointer_cast<TTypeParam<T>>(param)->getValue();
                } else {
                    throw std::runtime_error("Type mismatch for parameter");
                }
            }

            template<typename T>
            ParamAccessor &operator=(T newValue);

        private:
            std::shared_ptr<TTypeParamBase> param;
        };

    public:
        template<typename... Args>
        explicit TTypeParamHandler(Args... args) {
            addParameters(args...);
        }

        TTypeParamHandler(std::initializer_list<std::pair<std::string, TInitParamTypes>> args);

        void addParameter(const std::string &name, const TInitParamTypes &value);

        template<typename T>
        void addParameter(const std::string &name, T value) {
            mParameters[name] = std::make_shared<TTypeParam<T>>(value);
        }

        template<typename T>
        void setParameter(const std::string &name, T value);

        ParamAccessor operator[](const std::string &name);

        void merge(const TTypeParamHandler &other);

        void merge(std::initializer_list<std::pair<std::string, TInitParamTypes>> args);

        [[nodiscard]] std::map<std::string, std::string>
            getParamValues(std::initializer_list<std::string> paramNames) const;

        void printParameters() const;

    public:
        int stringToInt(const std::string &str);

        std::string intToHexString(int value);

    private:
        void addParameters() {}

        template<typename T, typename... Args>
        void addParameters(const std::string &name, T value, Args... args) {
            addParameter(name, value);
            addParameters(args...);
        }

    protected:
        std::unordered_map<std::string, std::shared_ptr<TTypeParamBase>> mParameters;
    };

};


#endif //TUTILS_TTYPEPARAMHANDLER_H
