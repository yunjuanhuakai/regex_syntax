cmake_minimum_required(VERSION 3.4)
project(regex_syntax)

set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

include_directories(include/)

file(GLOB SOURCE_FILES include/*.h src/*.cpp)

add_executable(regex_syntax ${SOURCE_FILES} test.cpp)
