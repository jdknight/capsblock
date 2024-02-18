# Copyright jdknight

cmake_minimum_required(VERSION 3.11)
project(capsblock)

set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${BASE_DIR}/src)
set(SRC_DIR ${BASE_DIR}/src/capsblock)
include(${BASE_DIR}/support.cmake)

include(${BASE_DIR}/sources.cmake)
include_directories(${INC_DIR})

add_executable(capsblock WIN32 ${CAPSBLOCK_SRCS})
install (TARGETS capsblock RUNTIME DESTINATION bin)
