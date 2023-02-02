cmake_minimum_required(VERSION 3.10.2)

# Project
project("DLXLibTest" CXX)

# Files
file(GLOB_RECURSE DLXLIB_TEST_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

phi_add_executable(
  NAME
  ${PROJECT_NAME}
  FOLDER
  "Tests"
  SOURCES
  ${DLXLIB_TEST_SOURCES}
  PRIVATE_LINK_LIBRARIES
  DLXLib
  Phi::Test)

phi_add_test(TARGET ${PROJECT_NAME})
