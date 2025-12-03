/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TConfig.h
   Author : tao.jing
   Date   : 2023/11/26
   Brief  : 
**************************************************************************/
#ifndef BREAKERMEA_TCONFIG_H
#define BREAKERMEA_TCONFIG_H

#include "TSingleton.h"
#include "TError.h"
#include "TIniHandler.h"
#include "TJsonHandler.h"
#include <yaml-cpp/yaml.h>

#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include <initializer_list>
#include "TaoUtilDefine.h"


#define GET_STR_CONFIG(...) TBase::TConfig::instance().getConfig<std::string>({__VA_ARGS__})
#define GET_BOOL_CONFIG(...) TBase::TConfig::instance().getConfig<bool>({__VA_ARGS__})
#define GET_INT_CONFIG(...) TBase::TConfig::instance().getConfig<int>({__VA_ARGS__})
#define GET_FLOAT_CONFIG(...) TBase::TConfig::instance().getConfig<float>({__VA_ARGS__})

#define GET_ARR_STR_CONFIG(index, ...) TBase::TConfig::instance().getArrayConfig<std::string>({__VA_ARGS__}, index)
#define GET_ARR_BOOL_CONFIG(index, ...) TBase::TConfig::instance().getArrayConfig<bool>({__VA_ARGS__}, index)
#define GET_ARR_INT_CONFIG(index, ...) TBase::TConfig::instance().getArrayConfig<int>({__VA_ARGS__}, index)
#define GET_ARR_FLOAT_CONFIG(index, ...) TBase::TConfig::instance().getArrayConfig<float>({__VA_ARGS__}, index)

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace TBase {

    class TIniHandler;

    class TAO_UTIL_API TConfig : public TSingleton<TConfig> {

        friend class TBase::TSingleton<TConfig>;

    protected:
        TConfig();

    public:

        ~TConfig() override;

        template<typename T>
        T getConfig(std::initializer_list<const std::string> keys);

        template<typename T>
        T getArrayConfig(std::initializer_list<const std::string> keys, std::size_t index);

        TBaseError getSeqNodeLen(std::initializer_list<const std::string> keys, std::size_t &seq_len);

        // Yaml
        void addYamlConfigFile(const std::string &yaml_file_path);

        // Ini
        void addIniConfigFile(const std::string &ini_file_path);

        // Json
        void addJsonConfigFile(const std::string &json_file_path);

    private:
        static std::string concatKeys(std::initializer_list<const std::string> keys);

        // Yaml
        YAML::Node getYamlNode(std::initializer_list<const std::string> keys);

        // Ini
        TBase::TIniHandler* getIniHandler(std::initializer_list<const std::string> keys);

        // Json
        TBase::TJsonHandler* getJsonHandler(std::initializer_list<const std::string> keys);

    private:
        class Impl;
        std::unique_ptr<Impl> mImpl;
    };

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //BREAKERMEA_TCONFIG_H
