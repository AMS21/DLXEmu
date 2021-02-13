include(Environment)

if(NOT ${DLXEMU_FUZZ_BUILD})
  add_subdirectory("DLXLibTest")
endif()

if(PHI_COMPILER_CLANG AND PHI_PLATFORM_LINUX)
  add_subdirectory("Fuzzing")
endif()
