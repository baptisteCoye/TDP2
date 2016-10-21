cmake_minimum_required(VERSION 2.8.11)
project(TDP2_PRCD)

INCLUDE_DIRECTORIES(
  ${RAVCI_BINARY_DIR} 
  ${RAVCI_BINARY_DIR}/src 
  ${RAVCI_SOURCE_DIR} 
  ${RAVCI_SOURCE_DIR}/src 
  )

add_executable(test_seq test_sequentiel.c)