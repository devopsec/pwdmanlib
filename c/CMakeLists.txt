cmake_minimum_required(VERSION 3.6)
project(c)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES library.c library.h)
add_library(c ${SOURCE_FILES})