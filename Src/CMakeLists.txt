cmake_minimum_required(VERSION 3.26)
project(FireApp)

# ---- Project Paths ----
IF (NOT EP_DIR)
    set (EP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/)
ELSE()
    set (EP_DIR ${EP_DIR})
ENDIF()

set (EP_MAIN_SRC_DIR ${EP_DIR}/Src/)
set (THIRD_PARTY_PATH ${EP_DIR}/ThirdParty/)


add_subdirectory(${EP_MAIN_SRC_DIR})