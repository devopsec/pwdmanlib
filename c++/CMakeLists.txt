cmake_minimum_required(VERSION 3.6)
project(c__)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES library.cpp library.h)
add_library(c__ ${SOURCE_FILES})