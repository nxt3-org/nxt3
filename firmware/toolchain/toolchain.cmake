# Specify OS
SET(CMAKE_SYSTEM_NAME      Linux)
SET(CMAKE_SYSTEM_VERSION   2.6.33)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# Specify compiler
SET(CMAKE_C_COMPILER   armv5-linux-musleabi-gcc)
SET(CMAKE_CXX_COMPILER armv5-linux-musleabi-g++)

# Specify flags
SET(CMAKE_C_FLAGS   "-march=armv5t -mtune=arm926ej-s" CACHE STRING "C   compiler flags" FORCE)
SET(CMAKE_CXX_FLAGS "-march=armv5t -mtune=arm926ej-s" CACHE STRING "CXX compiler flags" FORCE)
