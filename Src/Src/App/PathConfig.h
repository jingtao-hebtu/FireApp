/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : PathConfig.h
   Author : tao.jing
   Date   : 2023/12/11
   Brief  :
**************************************************************************/
#ifndef BREAKERMEA_TFPathConfig_H
#define BREAKERMEA_TFPathConfig_H

#include "TSingleton.h"
#include "TParamHandler.h"



#define TFPathParam(param_name) TF::TFPathConfig::instance()[param_name]
#define TFPathTypeParam(param_type, param_name) static_cast<param_type>(TFPathParam(param_name))


#define TbPathAddParam(path_name, path_value) TF::TFPathConfig::instance().addConfigPath(path_name, path_value)


namespace TF {

    class TFPathConfig
            : public TBase::TSingleton<TFPathConfig>, public TBase::TParamHandler {

        friend class TSingleton;

    public:
        void initConfigPath();

        void addConfigPath(const std::string &path_name, const std::string &path_value);

    protected:
        TFPathConfig();

    private:
        static void validDir(const std::string &dir);

    private:
        void initParams();

    };

};


#endif //BREAKERMEA_TFPathConfig_H
