
include_directories(${TUTIL_THIRD_LIB_PATH}/Inc)

if (UNIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(T_UTIL_LIB TUtilLibd)
        set(T_UTIL_LIB_PATH "/home/fire/project/TUtilLib/build/bin_debug/TUtilLib/")
    else ()
        set(T_UTIL_LIB TUtilLib)
        set(T_UTIL_LIB_PATH "/home/fire/project/TUtilLib/build/bin_release/TUtilLib/")
    endif ()
elseif (WIN32)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(T_UTIL_LIB TUtilLibd)
        set(T_UTIL_LIB_PATH "E:/Code/Cpp/Base/TUtilLib_Github/build/bin_debug/TUtilLib/")
    else ()
        set(T_UTIL_LIB TUtilLib)
        set(T_UTIL_LIB_PATH "E:/Code/Cpp/Base/TUtilLib_Github/build/bin_release/TUtilLib/")
    endif ()
endif()

link_directories(${T_UTIL_LIB_PATH})



