cmake_minimum_required(VERSION 3.8)
project(polinomtest)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp stdafx.h funcs.h Cclass.h globals.h Cclass.cpp chainreduction1.cpp chainreduction2.cpp pairconnection.cpp)
add_executable(polinomtest ${SOURCE_FILES})