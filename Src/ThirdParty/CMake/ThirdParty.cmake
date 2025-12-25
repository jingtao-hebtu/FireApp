# *  @Copyright (c) tao.jing
# *
# *

IF (NOT THIRD_PARTY_PATH)
    set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR})
ELSE ()
    set(THIRD_PARTY_PATH ${THIRD_PARTY_PATH})
ENDIF ()


# Qt
set(QT_LIB_PATH ${THIRD_PARTY_PATH}/Qt)
include(${QT_LIB_PATH}/CMake/Qt.cmake)

# FFMpeg
set(T_FFMPEG_DIR ${THIRD_PARTY_PATH}/TFFMpeg)
include(${T_FFMPEG_DIR}/CMake/TFFMpeg.cmake)

# TVideoSystem
set(T_VIDEO_SYSTEM_DIR ${THIRD_PARTY_PATH}/TVideoSystem)
include(${T_VIDEO_SYSTEM_DIR}/CMake/TVideoSystem.cmake)

# TUtilLib
set(TUTIL_THIRD_LIB_PATH ${THIRD_PARTY_PATH}/TUtilLib)
include(${TUTIL_THIRD_LIB_PATH}/CMake/TUtilLib.cmake)

# OpenCV
set(OPENCV_LIB_INSTALL_PATH ${THIRD_PARTY_PATH}/OpenCV)
include(${OPENCV_LIB_INSTALL_PATH}/CMake/OpenCV.cmake)

# OnnxRuntime
set(T_ONNX_RT_LIB_PATH ${THIRD_PARTY_PATH}/OnnxRuntimeGPU_1_23)
include(${T_ONNX_RT_LIB_PATH}/CMake/OnnxRuntime.cmake)

# Libuvc
set(T_LIBUVC_LIB_PATH ${THIRD_PARTY_PATH}/Libuvc)
include(${T_LIBUVC_LIB_PATH}/CMake/Libuvc.cmake)

# TrtYolo
set(TRT_YOLO_LIB_PATH ${THIRD_PARTY_PATH}/TrtYolo)
include(${TRT_YOLO_LIB_PATH}/CMake/TrtYolo.cmake)