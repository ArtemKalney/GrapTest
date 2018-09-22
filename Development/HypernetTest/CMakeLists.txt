cmake_minimum_required(VERSION 3.9)
project(HypernetTest)

set(CMAKE_CXX_STANDARD 11)

add_executable(HyperNetTest Main.cpp Stdafx.h Funcs.h Globals.h Branch.h PairConnectivity.cpp Branch.cpp Hypernet.h Hypernet.cpp GenCombinations.cpp)