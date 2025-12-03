# *  @Copyright (c) tao.jing
# *
# *
cmake_minimum_required(VERSION 3.26)
project(FireApp)



# Third Party
include(${THIRD_PARTY_PATH}/CMake/ThirdParty.cmake)


# Set sources and headers
file(GLOB_RECURSE EP_MAIN_SRC_FILES "${EP_MAIN_SRC_DIR}/*.cpp")

set(INCLUDE_DIRS "")
foreach (FILE_PATH ${EP_MAIN_SRC_FILES})
    get_filename_component(DIR_PATH ${FILE_PATH} PATH)
    list(APPEND INCLUDE_DIRS ${DIR_PATH})
endforeach ()

list(REMOVE_DUPLICATES INCLUDE_DIRS)

set(EP_MAIN_SRC_FILES "${EP_MAIN_SRC_FILES};${T_FFMPEG_SRC};${T_VIDEO_SYSTEM_SRC}")

set(EP_MAIN_SRC_FILES "${EP_MAIN_SRC_FILES};${EP_MAIN_SRC_DIR}/Res/rc.qrc;")


add_executable(${PROJECT_NAME} #WIN32
        ${EP_MAIN_SRC_FILES}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

set(APP_ICON_RESOURCE "${EP_MAIN_SRC_DIR}/Res/app_icon.rc")
target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_RESOURCE})


target_link_libraries(${PROJECT_NAME}
        ${QT_LIBS}
        ${T_FFMPEG_LIBS}
        ${T_UTIL_LIB}
)


if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(LINK_LIB_TYPE "Debug")
        set(DEBUG_SUFFIX "d")
    else ()
        set(LINK_LIB_TYPE "Release")
        set(DEBUG_SUFFIX)
    endif ()
    # TUtilLib
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${T_UTIL_LIB_PATH}/${T_UTIL_LIB}.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    # Qt
    set(QT_INSTALL_PATH "${Qt6_Base_DIR}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
    endif ()
    foreach (QT_LIB ${QT_LIBS})
        string(REPLACE "Qt::" "" QT_LIB_NAME ${QT_LIB})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB_NAME}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()