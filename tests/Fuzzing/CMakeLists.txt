find_package(Threads)

include(CompilerOptimizations)
include(Sanitizers)

# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run") # Default of 10 seconds

# Dictionary
add_executable(generate_dictionary_app "src/GenerateDictionary.cpp")
target_link_libraries(generate_dictionary_app PRIVATE DLXLib Threads::Threads)

add_custom_target(
  generate_dictionary
  COMMENT "Generating fuzzing dictionary"
  COMMAND generate_dictionary_app)

add_dependencies(generate_dictionary generate_dictionary_app)

# Convert structured text
add_executable(convert_structured_text "src/ConvertStructuredText.cpp")
target_link_libraries(convert_structured_text PRIVATE DLXLib Threads::Threads)
target_include_directories(convert_structured_text PRIVATE "include")

# Fuzzers
function(define_fuzzer)
  cmake_parse_arguments(df "ASCII" "NAME" "SOURCE;LIBRARIES;DEPENDENCY;RUN_ARGS" ${ARGN})

  add_executable(${df_NAME} ${df_SOURCE})
  target_link_libraries(${df_NAME} PRIVATE Threads::Threads -fsanitize=fuzzer ${df_LIBRARIES})
  target_compile_options(${df_NAME} PRIVATE -fsanitize=fuzzer)
  target_include_directories(${df_NAME} PRIVATE "include")
  if(df_DEPENDENCY)
    add_dependencies(${df_NAME} ${df_DEPENDENCY})
  endif()
  # Don't inline functions defined as always inline
  target_compile_definitions(${df_NAME} PRIVATE PHI_CONFIG_ALWAYS_INLINE_OVERWRITE=inline)

  phi_target_enable_optimizations(TARGET ${df_NAME} PSO "native")
  # phi_target_enable_sanitizer(TARGET ${df_NAME} SANITIZERS "address;undefined")

  # Tests
  add_test(NAME ${df_NAME}_run COMMAND ${df_NAME} -max_total_time=${FUZZ_RUNTIME} ${df_RUN_ARGS})
  if(df_ASCII)
    add_test(NAME ${df_NAME}_run_ascii_only COMMAND ${df_NAME} -max_total_time=${FUZZ_RUNTIME}
                                                    -only_ascii=1 ${df_RUN_ARGS})
  endif()

  if(generate_dictionary IN_LIST df_DEPENDENCY)
    add_test(NAME ${df_NAME}_run_with_dict
             COMMAND ${df_NAME} -max_total_time=${FUZZ_RUNTIME}
                     -dict=${CMAKE_CURRENT_BINARY_DIR}/Dictionary.txt ${df_RUN_ARGS})
    if(df_ASCII)
      add_test(NAME ${df_NAME}_run_with_dict_ascii_only
               COMMAND ${df_NAME} -max_total_time=${FUZZ_RUNTIME} -only_ascii=1
                       -dict=${CMAKE_CURRENT_BINARY_DIR}/Dictionary.txt ${df_RUN_ARGS})
    endif()
  endif()
endfunction()

# Parser
define_fuzzer(
  NAME
  fuzz_parser
  SOURCE
  "src/Parser.fuzz.cpp"
  LIBRARIES
  DLXLib
  DEPENDENCY
  generate_dictionary
  ASCII)

# Execution
define_fuzzer(
  NAME
  fuzz_execution
  SOURCE
  "src/Execution.fuzz.cpp"
  LIBRARIES
  DLXLib
  DEPENDENCY
  generate_dictionary
  ASCII)

# Structured Execution
define_fuzzer(NAME fuzz_structured_execution SOURCE "src/StructuredExecution.fuzz.cpp" LIBRARIES
              DLXLib)

# Syntax highlighting
define_fuzzer(
  NAME
  fuzz_syntax_highlighting
  SOURCE
  "src/SyntaxHighlighting.fuzz.cpp"
  LIBRARIES
  DLXEmuLib
  DEPENDENCY
  generate_dictionary
  ASCII)

# Structured Syntax highlighting
define_fuzzer(NAME fuzz_structured_syntax_highlighting SOURCE
              "src/StructuredSyntaxHighlighting.fuzz.cpp" LIBRARIES DLXEmuLib)

# Regression Test
define_fuzzer(NAME fuzz_regression SOURCE "src/RegressionTest.fuzz.cpp" LIBRARIES DLXLib)

# CodeEditor
define_fuzzer(NAME fuzz_code_editor SOURCE "src/CodeEditor.fuzz.cpp" LIBRARIES DLXEmuLib)

define_fuzzer(NAME fuzz_code_editor_verbose SOURCE "src/CodeEditor.fuzz.cpp" LIBRARIES DLXEmuLib)
target_compile_definitions(fuzz_code_editor_verbose PRIVATE "FUZZ_VERBOSE_LOG")

# EmulatorArgs
define_fuzzer(
  NAME
  fuzz_emulator_args
  SOURCE
  "src/EmulatorArgs.fuzz.cpp"
  LIBRARIES
  DLXEmuLib
  RUN_ARGS
  "-close_fd_mask=1"
  ASCII)
