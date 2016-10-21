cmake_minimum_required(VERSION 2.8.11)
project(TDP2_PRCD)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

add_subdirectory(src)
add_subdirectory(tests)