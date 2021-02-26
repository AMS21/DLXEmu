find_package(Threads)

# Dictionary
add_executable(generate_dictionary_app "src/GenerateDictionary.cpp")
target_link_libraries(generate_dictionary_app PRIVATE Phi::ProjectOptions DLXLib
                                                      Threads::Threads)

add_custom_target(
  generate_dictionary
  COMMENT "Generating fuzzing dictionary"
  COMMAND generate_dictionary_app)

add_dependencies(generate_dictionary generate_dictionary_app)

# Convert structured text
add_executable(convert_structured_text "src/ConvertStructuredText.cpp")
target_link_libraries(convert_structured_text PRIVATE Phi::ProjectOptions DLXLib Threads::Threads)
target_include_directories(convert_structured_text PRIVATE "include")

# Fuzzers
function(define_fuzzer name source libraries)
  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE Phi::ProjectOptions Threads::Threads -fsanitize=fuzzer ${libraries})
  target_compile_options(${name} PRIVATE -fsanitize=fuzzer)
  target_include_directories(${name} PRIVATE "include")
endfunction()

# Parser
define_fuzzer(fuzz_parser "src/Parser.fuzz.cpp" DLXLib)

# Execution
define_fuzzer(fuzz_execution "src/Execution.fuzz.cpp" DLXLib)

# Structured Execution
define_fuzzer(fuzz_structured_execution "src/StructuredExecution.fuzz.cpp" DLXLib)

# Syntax highlighting
define_fuzzer(fuzz_syntax_highlighting "src/SyntaxHighlighting.fuzz.cpp" DLXEmuLib)

# Structured Syntax highlighting
define_fuzzer(fuzz_structured_syntax_highlighting "src/StructuredSyntaxHighlighting.fuzz.cpp" DLXEmuLib)

# Regression Test
define_fuzzer(fuzz_regression "src/RegressionTest.fuzz.cpp" DLXLib)
