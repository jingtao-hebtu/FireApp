# *  @Copyright (c) tao.jing
# *
# *

if(UNIX)
    set(Qt6_Version "6.7.3")
    set(Qt6_Base_DIR "/data/Qt/${Qt6_Version}/gcc_arm64")
    set(Qt6_DIR "/data/Qt/${Qt6_Version}/gcc_arm64")
elseif(WIN32)
    set(Qt6_Version "6.9.1")
    set(Qt6_Base_DIR "D:/Software/Qt/${Qt6_Version}/msvc2022_64")
    set(Qt6_DIR "D:/Software/Qt/${Qt6_Version}/msvc2022_64")
endif()

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${Qt6_DIR};")
set(ENV{PATH} "${Qt6_DIR}/bin;$ENV{PATH}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        MultimediaWidgets
        PrintSupport
        Concurrent
        Network
        OpenGL
        Sql
        Svg
        SvgWidgets
        Charts
        OpenGLWidgets
        Xml
        Core5Compat
        SerialPort
        REQUIRED)

if (Qt6_FOUND)
    set(QT6_FOUND TRUE CACHE INTERNAL "Qt6 is found.")
else ()
    MESSAGE(FATAL_ERROR, "Qt6 not found.")
endif ()

set(QT_LIBS
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        Qt::MultimediaWidgets
        Qt::Concurrent
        Qt::Network
        Qt::OpenGL
        Qt::OpenGLWidgets
        Qt::Core5Compat
        Qt::PrintSupport
        Qt::Sql
        Qt::Xml
        Qt::Charts
        Qt::Svg
        Qt::SvgWidgets
        Qt::SerialPort
)