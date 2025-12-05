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

    set(LIBUSB_LIBS libusb-1.0 uvcstatic pthread)
elseif (UNIX)

endif ()

