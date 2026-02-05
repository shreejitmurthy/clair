cmake_minimum_required(VERSION 4.1)
project(clair)

set(CMAKE_CXX_STANDARD 20)

add_library(clair clair.cc)

add_subdirectory(example)
