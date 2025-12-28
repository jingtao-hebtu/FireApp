/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : PathConfig.cpp
   Author : tao.jing
   Date   : 2023/12/11
   Brief  :
**************************************************************************/
#include "PathConfig.h"
#include "TSysUtils.h"
#include "TFException.h"
#include "TLog.h"


namespace TF {

    TFPathConfig::TFPathConfig() {
        initParams();
    }

    void TFPathConfig::initParams() {
        (*this)["CurWorkPath"] = TBase::getCurWorkDir();
    }

    void TFPathConfig::initConfigPath() {
        // Check if AppConfigDir configured
        std::string app_config_dir = (*this)["AppConfigDir"];
        validDir(app_config_dir);

        (*this)["VisionMeaConfigDir"] = TBase::joinPath(app_config_dir, "VisionMea");
        validDir((*this)["VisionMeaConfigDir"]);

        (*this)["DatabaseConfigDir"] = TBase::joinPath(app_config_dir, "Database");
        validDir((*this)["DatabaseConfigDir"]);
    }

    void TFPathConfig::validDir(const std::string &dir) {
        if (!TBase::dirExists(dir)) {
            TF_LOG_THROW_RUNTIME("Config dir %s not found.", dir.c_str());
        }
    }

    void TFPathConfig::addConfigPath(const std::string &path_name, const std::string &path_value) {
        (*this)[path_name] = path_value;
    }

};


