# *  @Copyright (c) tao.jing
# *
# *
IF (NOT T_ONNX_RT_LIB_PATH)
    set (T_ONNX_RT_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})
ELSE()
    set (T_ONNX_RT_LIB_PATH ${T_ONNX_RT_LIB_PATH})
ENDIF()

# -------------- Compile CUDA for FP16 inference if needed  ------------------#
#option(USE_CUDA "Enable CUDA support" OFF)
set(USE_CUDA ON)
if (NOT APPLE AND USE_CUDA)
    #find_package(CUDA 11 REQUIRED)
    #find_package(CUDAToolkit 11 REQUIRED)
    find_package(CUDAToolkit 12 REQUIRED)
    set(CMAKE_CUDA_STANDARD 11)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    #include_directories(${CUDA_INCLUDE_DIRS})
    include_directories(${CUDAToolkit_INCLUDE_DIRS})
    message(STATUS "${CUDAToolkit_INCLUDE_DIRS}")
    add_definitions(-DUSE_CUDA)
else ()
    set(USE_CUDA OFF)
endif ()


set(ONNX_RUNTIME_ROOT "${T_ONNX_RT_LIB_PATH}")


include_directories(${ONNX_RUNTIME_ROOT}/include)


set(ONNX_RUNTIME_LIBS ${ONNX_RUNTIME_ROOT}/lib/onnxruntime.lib)
