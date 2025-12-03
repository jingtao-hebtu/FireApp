# *  @Copyright (c) tao.jing
# *
# *

set (OPENCV_LIB_FILE_NAME "opencv_world4110")
set (OPENCV_LOCAL_INSTALL_PATH "E:/Software/Library/OpenCV/opencv/build/x64/vc16")

set (OPENCV_LOCAL_INSTALL_LIB_PATH "${OPENCV_LOCAL_INSTALL_PATH}/lib")
set (OPENCV_LOCAL_INSTALL_BIN_PATH "${OPENCV_LOCAL_INSTALL_PATH}/bin")

# OpenCV
##[[
set(_OpenCV_REQUIRED_COMPONENTS core imgproc videoio highgui)
find_package(OpenCV COMPONENTS ${_OpenCV_REQUIRED_COMPONENTS} PATHS ${OPENCV_LOCAL_INSTALL_LIB_PATH} QUIET CONFIG)
if(OpenCV_FOUND)
    message(STATUS "OpenCV found.")
else()
    message(FATAL_ERROR "OpenCV not found: Not building exe.")
endif(OpenCV_FOUND)
#]]

# ---- OpenCV ---->
#[[
find_package(OpenCV REQUIRED PATHS "D:/Code/Library/OpenCV/opencv_48/build/x64/vc16/lib")
include_directories(${OpenCV_INCLUDE_DIRS})
message(STATUS ${OpenCV_INCLUDE_DIRS})
link_directories("D:/Code/Library/OpenCV/opencv_48/build/x64/vc16/lib")
IF (PROJECT_BUILD_TYPE MATCHES Debug)
    set(OPENCV_LIB opencv_world480d)
else ()
    set(OPENCV_LIB opencv_world480)
ENDIF ()
]]
# <---- OpenCV ----