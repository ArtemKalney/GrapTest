cmake_minimum_required(VERSION 3.8)
project(GraphTest)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Main.cpp Stdafx.h Funcs.h Cclass.h Globals.h Cclass.cpp ChainReduction1.cpp ChainReduction2.cpp PairConnection.cpp)
add_executable(GraphTest ${SOURCE_FILES})