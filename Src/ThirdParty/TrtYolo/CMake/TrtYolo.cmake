# *  @Copyright (c) tao.jing
# *
# *
if (WIN32)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(TENSORRT_YOLO_PATH "E:/Software/Library/TensorRTYolo/6_4/debug")
    else ()
        set(TENSORRT_YOLO_PATH "E:/Software/Library/TensorRTYolo/6_4/release")
    endif ()

    find_package(TensorRT-YOLO REQUIRED PATHS ${TENSORRT_YOLO_PATH})

    include_directories(${TensorRT-YOLO_INCLUDE_DIRS})
else ()
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(TENSORRT_YOLO_PATH "/home/fire/project/library/trtyolo/install/debug")
    else ()
        set(TENSORRT_YOLO_PATH "/home/fire/project/library/trtyolo/install/release")
    endif ()

    find_package(TensorRT-YOLO REQUIRED PATHS ${TENSORRT_YOLO_PATH})

    include_directories(${TensorRT-YOLO_INCLUDE_DIRS})
endif()