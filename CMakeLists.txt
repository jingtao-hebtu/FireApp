# *  @Copyright (c) tao.jing
# *
# *
cmake_minimum_required(VERSION 3.26)
project(FireApp)

# ---- Compilations ----
set (CMAKE_CXX_STANDARD 20)
add_definitions("-DUNICODE" "-D_UNICODE")


if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    add_compile_options(/wd4828)
else ()
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
endif ()

# ---- Project Management ----
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (BASE_PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR})


if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/bin_debug)
    set(EXECUTABLE_OUTPUT_PATH ${BASE_OUTPUT_DIR})
else()
    set(BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/bin_release)
    set(EXECUTABLE_OUTPUT_PATH ${BASE_OUTPUT_DIR})
endif()


add_subdirectory(Src)