cmake_minimum_required(VERSION 3.10)

project(NXT3  VERSION 0.1
              DESCRIPTION "NXT firmware for EV3"
              LANGUAGES C)

add_subdirectory(hal)
add_subdirectory(core)
