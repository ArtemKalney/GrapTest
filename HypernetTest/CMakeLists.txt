cmake_minimum_required(VERSION 3.9)
project(HyperNetTest)

set(CMAKE_CXX_STANDARD 11)

add_executable(HyperNetTest main.cpp stdafx.h funcs.h globals.h Branch.h pairconnection.cpp Branch.cpp Hypernet.h Hypernet.cpp)