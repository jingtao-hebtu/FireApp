/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TYamlHelper.h
   Author : tao.jing
   Date   : 2023/11/27
   Brief  : 
**************************************************************************/
#ifndef BREAKERMEA_TYAMLHELPER_H
#define BREAKERMEA_TYAMLHELPER_H

#include "TError.h"
#include <yaml-cpp/yaml.h>
#include "TaoUtilDefine.h"


namespace TBase::TYamlHelper {
    template<typename T>
    TAO_UTIL_API TBaseError getScalarValue(const std::string &yaml_file_path,
                              const std::string &key,
                              T &value);

    template<typename T>
    TAO_UTIL_API TBaseError getSeqValues(const std::string &yaml_file_path,
                            const std::string &key,
                            std::vector<T> &values);

    void recursiveFetchKey(const YAML::Node &node, std::vector<std::string> &keys);
};


#endif //BREAKERMEA_TYAMLHELPER_H
