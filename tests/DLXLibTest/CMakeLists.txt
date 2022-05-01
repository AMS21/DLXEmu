cmake_minimum_required(VERSION 3.10.2)

# Project
project("DLXLibTest" CXX)

# Files
file(
  GLOB
  DLXLIB_TEST_SOURCES
  "src/FloatRegister.test.cpp"
  "src/Instruction.test.cpp"
  "src/InstructionLibrary.test.cpp"
  "src/InstructionArgument.test.cpp"
  "src/InstructionInfo.test.cpp"
  "src/IntRegister.test.cpp"
  "src/MemoryBlock.test.cpp"
  "src/OpCode.test.cpp"
  "src/ParsedProgram.test.cpp"
  "src/ParseError.test.cpp"
  "src/Parser.test.cpp"
  "src/ParserUtils.test.cpp"
  "src/Processor.test.cpp"
  "src/ProcessorCrashes.test.cpp"
  "src/RegisterNames.test.cpp"
  "src/StatusRegister.test.cpp"
  "src/Token.test.cpp"
  "src/Tokenize.test.cpp"
  "src/TokenStream.test.cpp")
file(GLOB DLXLIB_TEST_HEADERS "include/StringHelper.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DLXLIB_TEST_SOURCES} ${DLXLIB_TEST_HEADERS})

add_executable("DLXLibUnittest" ${DLXLIB_TEST_SOURCES} ${DLXLIB_TEST_HEADERS})

target_link_libraries("DLXLibUnittest" PRIVATE DLXLib Catch2::Catch2WithMain)

target_include_directories("DLXLibUnittest" PRIVATE "include")

# target_compile_definitions("DLXLibUnittest" PRIVATE
# "_GLIBCXX_DEBUG;_LIBCPP_DEBUG=1;_ITERATOR_DEBUG_LEVEL=2")

set_target_properties("DLXLibUnittest" PROPERTIES FOLDER "Tests")
set_target_properties("DLXLibUnittest" PROPERTIES CXX_STANDARD 20)

phi_add_test(TARGET DLXLibUnittest)
