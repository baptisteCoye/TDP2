cmake_minimum_required(VERSION 2.8.11)
project(TDP2_PRCD)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

option(TDP2_BUILD_TESTS "Set to ON to build the functionalities tests" ON)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

add_subdirectory(src)

if(TDP2_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()