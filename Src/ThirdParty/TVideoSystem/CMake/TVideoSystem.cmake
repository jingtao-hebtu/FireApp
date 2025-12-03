# *  @Copyright (c) tao.jing
# *
# *

IF (NOT T_VIDEO_SYSTEM_DIR)
    set(T_VIDEO_SYSTEM_DIR ${CMAKE_CURRENT_SOURCE_DIR})
ELSE ()
    set(T_VIDEO_SYSTEM_DIR ${T_VIDEO_SYSTEM_DIR})
ENDIF ()


file(GLOB_RECURSE T_VIDEO_SYSTEM_SRC "${T_VIDEO_SYSTEM_DIR}/*.cpp")

set(INCLUDE_DIRS "")
foreach (FILE_PATH ${T_VIDEO_SYSTEM_SRC})
    get_filename_component(DIR_PATH ${FILE_PATH} PATH)
    list(APPEND INCLUDE_DIRS ${DIR_PATH})
endforeach ()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

include_directories(${INCLUDE_DIRS})