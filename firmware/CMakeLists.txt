cmake_minimum_required(VERSION 3.10)

project(NXT3  VERSION 0.1
              DESCRIPTION "NXT firmware for EV3"
              LANGUAGES C)

add_definitions(-D_GNU_SOURCE=1)
SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=arm926ej-s")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mcpu=arm926ej-s")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -mcpu=arm926ej-s")

add_subdirectory(hal)
add_subdirectory(core)
