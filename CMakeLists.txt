cmake_minimum_required(VERSION 3.0)

project(muduo C CXX)

# set(CMAKE_CXX_FLAGS)

include_directories(${CMAKE_SOURCE_DIR})

set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(muduo/base)
