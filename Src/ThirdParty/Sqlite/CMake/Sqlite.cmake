# *  @Copyright (c) tao.jing
# *
# *

cmake_minimum_required(VERSION 3.22)

if (WIN32)
  if (NOT SQLITE_LIB_PATH)
    set (SQLITE_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR})
  else()
    set (SQLITE_LIB_PATH ${SQLITE_LIB_PATH})
  endif()

  include_directories(${SQLITE_LIB_PATH}/Inc)
  include_directories(${SQLITE_LIB_PATH}/Inc/SQLiteCpp)

  IF(CMAKE_BUILD_TYPE MATCHES DEBUG OR
          CMAKE_BUILD_TYPE MATCHES Debug OR
          CMAKE_BUILD_TYPE MATCHES debug)
    set(THIRD_PARTY_SQLITE_LIB_PATH ${SQLITE_LIB_PATH}/Lib/debug)
  else()
    set(THIRD_PARTY_SQLITE_LIB_PATH ${SQLITE_LIB_PATH}/Lib/release)
  ENDIF()

  # Sqlite lib
  link_directories(${THIRD_PARTY_SQLITE_LIB_PATH})
  set(SQLTIE_LIBS "sqlite3;SQLiteCpp;")
else()
  set(SQLITE_CPP_BASE_DIR "/home/fire/project/library/sqlitecpp/install/3_3")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SQLiteCpp_DIR "${SQLITE_CPP_BASE_DIR}/debug")
  else()
    set(SQLiteCpp_DIR "${SQLITE_CPP_BASE_DIR}/release")
  endif()

  find_package(SQLiteCpp REQUIRED PATHS ${SQLiteCpp_DIR})

  include_directories(${SQLiteCpp_DIR}/include)
  include_directories(${SQLiteCpp_DIR}/include/SQLiteCpp)
endif()




