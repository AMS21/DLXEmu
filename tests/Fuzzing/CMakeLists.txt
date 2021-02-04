find_package(Threads)

# Dictionary
add_executable(generate_dictionary_app "src/GenerateDictionary.cpp")
target_link_libraries(generate_dictionary_app PRIVATE Phi::ProjectOptions DLXEmuLib
                                                      Threads::Threads)

add_custom_target(
  generate_dictionary
  COMMENT "Generating fuzzing dictionary"
  COMMAND generate_dictionary_app)

add_dependencies(generate_dictionary generate_dictionary_app)

# Parser
add_executable(fuzz_parser "src/Parser.fuzz.cpp")
target_link_libraries(fuzz_parser PRIVATE Phi::ProjectOptions DLXEmuLib Threads::Threads
                                          -fsanitize=fuzzer)
target_compile_options(fuzz_parser PRIVATE -fsanitize=fuzzer)

# Execution
add_executable(fuzz_execution "src/Processor.fuzz.cpp")
target_link_libraries(fuzz_execution PRIVATE Phi::ProjectOptions DLXEmuLib Threads::Threads
                                             -fsanitize=fuzzer)
target_compile_options(fuzz_execution PRIVATE -fsanitize=fuzzer)

# Regression Test
add_executable(fuzz_regression "src/RegressionTest.fuzz.cpp")
target_link_libraries(fuzz_regression PRIVATE Phi::ProjectOptions DLXEmuLib Threads::Threads
                                              -fsanitize=fuzzer)
target_compile_options(fuzz_regression PRIVATE -fsanitize=fuzzer)

# Failure
add_executable(fuzz_failure "src/Failure.fuzz.cpp")
target_link_libraries(fuzz_failure PRIVATE Phi::ProjectOptions Threads::Threads -fsanitize=fuzzer)
target_compile_options(fuzz_failure PRIVATE -fsanitize=fuzzer)

add_executable(fuzz_timeout "src/Timeout.fuzz.cpp")
target_link_libraries(fuzz_timeout PRIVATE Phi::ProjectOptions Threads::Threads -fsanitize=fuzzer)
target_compile_options(fuzz_timeout PRIVATE -fsanitize=fuzzer)
