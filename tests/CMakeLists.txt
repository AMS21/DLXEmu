include(Environment)

if (PHI_PLATFORM_EMSCRIPTEN)
     # Force js executable suffix for tests so node can run them
     set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()

if(NOT ${DLXEMU_FUZZ_BUILD})
  add_subdirectory("DLXLibTest")
endif()

if(PHI_COMPILER_CLANG AND PHI_PLATFORM_LINUX)
  add_subdirectory("Fuzzing")
endif()
