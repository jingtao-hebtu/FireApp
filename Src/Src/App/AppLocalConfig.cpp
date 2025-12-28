/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : AppLocalConfig.h
   Author : tao.jing
   Date   : 2025/11/22
   Brief  :
**************************************************************************/
#include "AppLocalConfig.h"
#include "TConfig.h"
#include "TYamlHelper.h"
#include "TSysUtils.h"
#include "TFException.h"
#include "PathConfig.h"
#include "TLog.h"


void TF::AppLocalConfig::initConfigs() {
    initAppConfig();
    LOG_F(INFO, "[AppConfig] After initConfigs.");
}

void TF::AppLocalConfig::initAppConfig() {
    // --- Search for App config file: TFConfigs/TFConfigs.yml
    bool found = false;
    std::string file_path("TFConfigs/TFConfigs.yml");
    file_path = TBase::searchFileInParentDirs(file_path, found);
    if (!found) {
        TF_LOG_THROW_RUNTIME("Config file %s not found.", file_path.c_str());
    }
    TBase::TConfig::instance().addYamlConfigFile(file_path);

    auto app_config_dir = TBase::getDirFromFilePath(file_path);
    TFPathParam("AppConfigDir") = app_config_dir;
}
