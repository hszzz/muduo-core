cmake_minimum_required(VERSION 3.0)

project(muduo-core C CXX)

set (LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

aux_source_directory(. SRC_LIST)

add_library(muduo_core SHARED ${SRC_LIST})
target_link_libraries(muduo_core pthread)

add_subdirectory(tests)
