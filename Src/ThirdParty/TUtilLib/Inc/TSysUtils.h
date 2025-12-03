/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : SysUtils.h
   Author : tao.jing
   Date   : 2023/11/12
   Brief  : 
**************************************************************************/
#ifndef BREAKERMEA_TSYSUTILS_H
#define BREAKERMEA_TSYSUTILS_H


#include <cstdint>
#include <string>
#include <vector>
#include "TaoUtilDefine.h"


namespace TBase {
    TAO_UTIL_API std::string getUserName();

    // ----- Dir and file -----
    TAO_UTIL_API bool dirExists(const std::string &dir_path);

    TAO_UTIL_API bool fileExists(const std::string &file_name);

    TAO_UTIL_API bool createDir(const std::string &dir_path);

    TAO_UTIL_API bool delDir(const std::string &dir_path);

    TAO_UTIL_API std::string getCurWorkDir();

    TAO_UTIL_API std::string joinPath(const std::vector<std::string> &paths);

    TAO_UTIL_API std::string joinPath(const std::vector<std::string> &paths, const std::string &file_name);

    TAO_UTIL_API std::string joinPath(const std::string &path, const std::string &file_name);

    TAO_UTIL_API std::string searchFileInParentDirs(const std::string &file_name, bool &found,
                                                    std::uint8_t search_folder_levels = 5);

    TAO_UTIL_API std::string getDirFromFilePath(const std::string &file_path);

    // ----- Date and time -----
    TAO_UTIL_API std::string getDate(char split_char = '_');

    TAO_UTIL_API std::string getTime(char split_char = ':');

    // ----- Process -----
    TAO_UTIL_API void killProcessByName(const char *target_process_name);

    // ----- Net -----
    TAO_UTIL_API int getValidPort();
};


#endif //BREAKERMEA_TSYSUTILS_H
