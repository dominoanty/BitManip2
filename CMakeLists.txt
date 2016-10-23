cmake_minimum_required(VERSION 3.6)
project(BitManip2)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm -fopenmp")

set(SOURCE_FILES libs/qdbmp.c)
set(SOURCE_FILES libs/qdbmp.h)
set(SOURCE_FILES filter.c)
set(SOURCE_FILES resize.c)
set(SOURCE_FILES main.c)
add_executable(BitManip2 ${SOURCE_FILES})
