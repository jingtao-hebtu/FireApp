# *  @Copyright (c) tao.jing
# *
# *

IF (NOT T_LIBUVC_LIB_PATH)
    set(T_LIBUVC_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})
ELSE ()
    set(T_LIBUVC_LIB_PATH ${T_LIBUVC_LIB_PATH})
ENDIF ()

if (WIN32)
    include_directories(${T_LIBUVC_LIB_PATH}/Inc)
    include_directories(${T_LIBUVC_LIB_PATH}/Inc/pthread)

    link_directories(${T_LIBUVC_LIB_PATH}/Lib/Win)

    set(LIBUVC_LIBS libusb-1.0 uvcstatic pthread)
elseif (UNIX)
    # Linux only part
    if(UNIX AND NOT APPLE)
        find_package(Threads REQUIRED)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)

        set(LIBUVC_LIB_PATH "/home/fire/project/library/libuvc/install/release")

        include_directories(${LIBUVC_LIB_PATH}/include)
        link_directories(${LIBUVC_LIB_PATH}/lib)
        set(LIBUVC_LIBS uvcstatic PkgConfig::LIBUSB Threads::Threads)
    endif()
endif ()

