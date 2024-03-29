cmake_minimum_required(VERSION 3.10.2)

# Policy
if(${CMAKE_VERSION} VERSION_LESS 3.25)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
  cmake_policy(VERSION 3.25)
endif()

# Settings
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(DLXEMU_BUILD_TESTS "" ON)
option(DLXEMU_FUZZ_BUILD "" OFF)
option(DLXEMU_RUN_FUZZ_TESTS "" ON)
option(DLXEMU_BUILD_BENCHMARKS "" OFF)
option(DLXEMU_USE_GLAD "Use the glad library for OpenGL" ON)

# Build modes
option(DLXEMU_ENABLE_ASAN OFF)
option(DLXEMU_ENABLE_LSAN OFF)
option(DLXEMU_ENABLE_TSAN OFF)
option(DLXEMU_ENABLE_UBSAN OFF)
option(DLXEMU_COVERAGE_BUILD "" OFF)

# Set version
set(DLXEMU_VERSION_MAJOR 0)
set(DLXEMU_VERSION_MINOR 1)
set(DLXEMU_VERSION_PATCH 0)

# Get git info
execute_process(
  COMMAND git log --format=%H -n 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE DLXEMU_GIT_SHA_FULL
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git log --format=%h -n 1
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE DLXEMU_GIT_SHA_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND git branch --show-current
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE DLXEMU_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get uname info
execute_process(
  COMMAND uname -s -r -m -o
  OUTPUT_VARIABLE DLXEMU_UNAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Linux Standand base info
execute_process(
  COMMAND lsb_release --id --short
  OUTPUT_VARIABLE DLXEMU_LSB_ID
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND lsb_release --description --short
  OUTPUT_VARIABLE DLXEMU_LSB_DESCRIPTION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND lsb_release --release --short
  OUTPUT_VARIABLE DLXEMU_LSB_RELEASE
  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(
  COMMAND lsb_release --codename --short
  OUTPUT_VARIABLE DLXEMU_LSB_CODENAME
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Sanitize values
if(DLXEMU_LSB_DESCRIPTION)
  string(REPLACE "\"" "" DLXEMU_LSB_DESCRIPTION ${DLXEMU_LSB_DESCRIPTION})
endif()

# Project
project(
  "DLXEmu"
  LANGUAGES CXX C
  VERSION ${DLXEMU_VERSION_MAJOR}.${DLXEMU_VERSION_MINOR}.${DLXEMU_VERSION_PATCH})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/Phi/scripts/cmake)

include(internal/IncludeGuard)
include(Environment)

# Build js files when using emscripten
if(PHI_PLATFORM_EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".js")
  add_link_options(
    -sASSERTIONS=1
    -sDISABLE_EXCEPTION_CATCHING=1
    -sERROR_ON_UNDEFINED_SYMBOLS=0
    -sFETCH=1
    -sFULL_ES2=1
    -sFULL_ES3=1
    -sUSE_GLFW=3)
endif()

# Ensure submodules were cloned correctly
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/Phi/CMakeLists.txt")
  message(
    FATAL_ERROR
      "The required submodules were not correnctly cloned. You can recover by manually calling `git submodule update --init --recursive`"
  )
endif()

# Dependencies
add_subdirectory(external)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Targets DLXLib
add_subdirectory(DLXLib)

# DLXEmu
add_subdirectory(DLXEmu)

# Tests
if(${DLXEMU_BUILD_TESTS})
  enable_testing()
  include(Testing)

  add_subdirectory(tests)
endif()

if(${DLXEMU_BUILD_BENCHMARKS})
  add_subdirectory(bench)
endif()

# Configure

include(ProjectOptions)

set(configure_options
    DEBUG_FLAGS
    OPTIMIZATION_FLAGS
    WARNINGS
    PEDANTIC
    FPM_PRECISE
    NO_EXCEPTIONS
    LTO
    STANDARD
    "latest"
    EXTERNAL
    "external")

# Warnings as errors
if(NOT PHI_PLATFORM_EMSCRIPTEN)
  set(configure_options ${configure_options} WARNINGS_AS_ERRORS)
endif()

# Sanitizer options
set(san_options)
if(DLXEMU_ENABLE_ASAN)
  set(san_options ${san_options} "address")
endif()
if(DLXEMU_ENABLE_LSAN)
  set(san_options ${san_options} "leak")
endif()
if(DLXEMU_ENABLE_TSAN)
  set(san_options ${san_options} "thread")
endif()
if(DLXEMU_ENABLE_UBSAN)
  # Sadly some of our dependencies rely on undefined behavior so we disable the checks here
  if(PHI_COMPILER_CLANG OR PHI_COMPILER_GCC)
    add_compile_options(
      "-fsanitize=undefined,float-divide-by-zero,local-bounds,nullability"
      "-fno-sanitize=float-cast-overflow"
      "-fno-common -fno-inline -fno-inline-functions -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-sanitize-recover=all"
    )
  endif()
endif()
if(DLXEMU_COVERAGE_BUILD)
  set(configure_options ${configure_options} COVERAGE)
endif()

# Fuzz build
if(DLXEMU_FUZZ_BUILD)
  set(configure_options ${configure_options} FUZZING_BUILD)
endif()

if(san_options)
  set(configure_options ${configure_options} SANITIZER ${san_options})

  message(STATUS "Enabled sanitizers: ${san_options}")
endif()

phi_configure_project(${configure_options})
