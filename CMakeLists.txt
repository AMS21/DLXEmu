cmake_minimum_required(VERSION 3.10.2)

# Settings
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(DLXEMU_BUILD_TESTS "" ON)
option(DLXEMU_FUZZ_BUILD "" OFF)
option(DLXEMU_RUN_FUZZ_TESTS "" ON)

# Project
project("DLXEmu" CXX C)

# Use latest standard available but at least C++-11
if(DEFINED CMAKE_CXX20_STANDARD_COMPILE_OPTION OR DEFINED CMAKE_CXX20_EXTENSION_COMPILE_OPTION)
  set(LatestCXXStandard 20)
elseif(DEFINED CMAKE_CXX17_STANDARD_COMPILE_OPTION OR DEFINED CMAKE_CXX17_EXTENSION_COMPILE_OPTION)
  set(LatestCXXStandard 17)
elseif(DEFINED CMAKE_CXX14_STANDARD_COMPILE_OPTION OR DEFINED CMAKE_CXX14_EXTENSION_COMPILE_OPTION)
  set(LatestCXXStandard 14)
else()
  set(LatestCXXStandard 11)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD ${LatestCXXStandard})

# Dependencies
add_subdirectory(external)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/Phi/scripts/cmake)

include(StandardProjectSettings)
include(Environment)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build html files when using emscripten
if(PHI_PLATFORM_EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_link_options(phi_project_options INTERFACE -sUSE_GLFW=3 -sFETCH=1 -sFULL_ES2=1 -sFULL_ES3=1 -sALLOW_MEMORY_GROWTH=1 -sASSERTIONS=1 -sDISABLE_EXCEPTION_CATCHING=1)
endif()

# Targets DLXLib
add_subdirectory(DLXLib)

# DLXEmu
add_subdirectory(DLXEmu)

# Tests
if(${DLXEMU_BUILD_TESTS} AND NOT PHI_PLATFORM_EMSCRIPTEN)
  enable_testing()

  add_subdirectory(tests)
endif()
