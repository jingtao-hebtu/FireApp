# Try pkg-config first
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_ZeroMQ QUIET libzmq)
endif()

# 找头文件 zmq.h
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.h
        HINTS ${PC_ZeroMQ_INCLUDE_DIRS}
)

# 找库 libzmq.so
find_library(ZeroMQ_LIBRARY
        NAMES zmq libzmq
        HINTS ${PC_ZeroMQ_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZeroMQ
        REQUIRED_VARS ZeroMQ_LIBRARY ZeroMQ_INCLUDE_DIR
)

if(ZeroMQ_FOUND)
    set(ZeroMQ_LIBRARIES    ${ZeroMQ_LIBRARY})
    set(ZeroMQ_INCLUDE_DIRS ${ZeroMQ_INCLUDE_DIR})
endif()
