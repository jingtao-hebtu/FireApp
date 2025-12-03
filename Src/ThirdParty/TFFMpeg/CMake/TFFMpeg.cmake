# *  @Copyright (c) tao.jing
# *
# *

IF (NOT T_FFMPEG_DIR)
    set(T_FFMPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR})
ELSE ()
    set(T_FFMPEG_DIR ${T_FFMPEG_DIR})
ENDIF ()

if (WIN32)
    add_definitions(-Dffmpeg)
    add_definitions(-Dffmpeg7)
    add_definitions(-Dvideoffmpeg)

    include_directories(${T_FFMPEG_DIR}/Inc)
    include_directories(${T_FFMPEG_DIR}/Lib/ffmpeg7/include)

    aux_source_directory(${T_FFMPEG_DIR}/Src T_FFMPEG_SRC)
    file(GLOB T_FFMPEG_HEADERS ${T_FFMPEG_DIR}/Inc/*.h)
    set(T_FFMPEG_SRC "${T_FFMPEG_SRC};${T_FFMPEG_HEADERS}")

    link_directories(${T_FFMPEG_DIR}/Lib/ffmpeg7/libwin64)

    set(T_FFMPEG_LIBS avformat avfilter avcodec swresample swscale avutil avdevice postproc opengl32 GLU32)
elseif (UNIX)
    add_definitions(-Dffmpeg)
    add_definitions(-Dffmpeg6)
    add_definitions(-Dvideoffmpeg)

    include_directories(${T_FFMPEG_DIR}/Inc)
    aux_source_directory(${T_FFMPEG_DIR}/Src T_FFMPEG_SRC)
    file(GLOB T_FFMPEG_HEADERS ${T_FFMPEG_DIR}/Inc/*.h)
    set(T_FFMPEG_SRC "${T_FFMPEG_SRC};${T_FFMPEG_HEADERS}")

    pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
    pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter)
    pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
    pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
    pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)
    pkg_check_modules(SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)
    set(T_FFMPEG_LIBS avformat avfilter avcodec swresample swscale avutil avdevice postproc)

endif ()

