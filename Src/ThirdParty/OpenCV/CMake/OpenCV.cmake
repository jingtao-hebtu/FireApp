# *  @Copyright (c) tao.jing
# *
# *
if (WIN32)

    set (OPENCV_LIB_FILE_NAME "opencv_world4110")
    set (OPENCV_LOCAL_INSTALL_PATH "E:/Software/Library/OpenCV/opencv/build/x64/vc16")

    set (OPENCV_LOCAL_INSTALL_LIB_PATH "${OPENCV_LOCAL_INSTALL_PATH}/lib")
    set (OPENCV_LOCAL_INSTALL_BIN_PATH "${OPENCV_LOCAL_INSTALL_PATH}/bin")

    set(_OpenCV_REQUIRED_COMPONENTS core imgproc videoio highgui)
    find_package(OpenCV COMPONENTS ${_OpenCV_REQUIRED_COMPONENTS} PATHS ${OPENCV_LOCAL_INSTALL_LIB_PATH} QUIET CONFIG)
    if(OpenCV_FOUND)
        message(STATUS "OpenCV found.")
    else()
        message(FATAL_ERROR "OpenCV not found: Not building exe.")
    endif(OpenCV_FOUND)

else ()

    find_package(OpenCV QUIET COMPONENTS core imgproc videoio dnn highgui)
    if (NOT OpenCV_FOUND)
        if (UNIX)
            pkg_check_modules(OPENCV QUIET IMPORTED_TARGET opencv4)
            if (NOT OPENCV_FOUND)
                message(FATAL_ERROR "OpenCV not found. Install libopencv-dev or provide OpenCV_DIR.")
            endif()
        else()
            message(FATAL_ERROR "OpenCV package configuration not found. Set OpenCV_DIR to your installation.")
        endif()
    endif()

endif()