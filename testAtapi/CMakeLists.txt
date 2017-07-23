cmake_minimum_required(VERSION 3.8)
project(testAtapi)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Cclass.h stdafx.h Cclass.cpp products.h products.cpp)
add_executable(testAtapi ${SOURCE_FILES})