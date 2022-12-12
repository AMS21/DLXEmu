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
file(GLOB DLXLIB_TEST_HEADERS "include/StringHelper.hpp" "include/Unary.hpp")

phi_add_executable(
  NAME
  ${PROJECT_NAME}
  FOLDER
  "Tests"
  SOURCES
  ${DLXLIB_TEST_SOURCES}
  HEADERS
  ${DLXLIB_TEST_HEADERS}
  PRIVATE_LINK_LIBRARIES
  DLXLib
  Phi::Test
  PRIVATE_INCLUDE_DIRS
  "include")

phi_add_test(TARGET ${PROJECT_NAME})
