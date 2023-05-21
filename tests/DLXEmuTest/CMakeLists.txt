cmake_minimum_required(VERSION 3.10.2)

# Project
project("DLXEmuTest" CXX)

# Files
file(GLOB DLXEMU_TEST_SOURCES "src/CodeEditor.test.cpp" "src/CodeEditorCrashes.test.cpp"
     "src/SetupImGui.cpp" "src/SetupImGui.test.cpp")
file(GLOB DLXEMU_TEST_HEADERS "include/SetupImGui.hpp")

phi_add_executable(
  NAME
  "${PROJECT_NAME}"
  FOLDER
  "Tests"
  SOURCES
  ${DLXEMU_TEST_SOURCES}
  HEADERS
  ${DLXEMU_TEST_HEADERS}
  PRIVATE_LINK_LIBRARIES
  DLXEmuLib
  Phi::Test
  PRIVATE_INCLUDE_DIRS
  "include")

phi_add_test(TARGET ${PROJECT_NAME})
