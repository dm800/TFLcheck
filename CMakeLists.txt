cmake_minimum_required(VERSION 4.0)
project(TFLcheck)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

add_executable(TFLcheck main.cpp)
