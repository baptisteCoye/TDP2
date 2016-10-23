cmake_minimum_required (VERSION 2.8.11)
project(TDP2_PRCD)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR} 
  ${CMAKE_BINARY_DIR}/src 
  ${CMAKE_SOURCE_DIR} 
  ${CMAKE_SOURCE_DIR}/src 
  ${MPI_INCLUDE_PATH}
  )

add_library(util SHARED util.c)

add_executable(driver driver.c)
target_link_libraries(driver util ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(driver PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(driver PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()