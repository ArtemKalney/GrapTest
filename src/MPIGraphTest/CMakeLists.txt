cmake_minimum_required(VERSION 3.9)
project(MPIGraphTest)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Cclass.cpp Cclass.h ChainReduction1.cpp ChainReduction2.cpp Funcs.h Globals.h Main.cpp MPI_pack.cpp PairConnection.cpp Stdafx.h ApplicationSettings.h)
add_executable(MPIGraphTest ${SOURCE_FILES})

#standard libraries
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

#other libraries
set(CMAKE_EXE_LINKER_FLAGS "-static -lpthread")

# Require MPI for this project:
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(MPIGraphTest ${MPI_LIBRARIES})