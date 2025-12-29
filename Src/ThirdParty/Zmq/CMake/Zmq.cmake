# *  @Copyright (c) tao.jing
# *
# *

cmake_minimum_required(VERSION 3.23)


if (UNIX)
  list(APPEND CMAKE_MODULE_PATH "${ZMQ_THIRD_LIB_PATH}/CMake")
  find_package(ZeroMQ REQUIRED)
  include_directories(${ZeroMQ_INCLUDE_DIR})
elseif (WIN32)
  set(ZeroMQ_DIR "E:/Software/Library/ZMQ/libzmq/install/4_3_5/release/CMake/")
  set(cppzmq_DIR "E:/Software/Library/ZMQ/cppzmq/install/4_9_0/release/share/cmake/cppzmq/")
  find_package(cppzmq REQUIRED path ${cppzmq_DIR})
  set(ZMQ_LIBS cppzmq)
endif()


