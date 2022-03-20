cmake_minimum_required(VERSION 3.10.2)

project(DLXLib LANGUAGES CXX)

file(
  GLOB
  DLXLIB_SOURCES
  "src/FloatRegister.cpp"
  "src/Instruction.cpp"
  "src/InstructionArgument.cpp"
  "src/InstructionImplementation.cpp"
  "src/InstructionInfo.cpp"
  "src/InstructionLibrary.cpp"
  "src/IntRegister.cpp"
  "src/Logger.cpp"
  "src/MemoryBlock.cpp"
  "src/OpCode.cpp"
  "src/ParsedProgram.cpp"
  "src/ParseError.cpp"
  "src/Processor.cpp"
  "src/RegisterNames.cpp"
  "src/Parser.cpp"
  "src/StatusRegister.cpp"
  "src/Token.cpp"
  "src/Tokenize.cpp"
  "src/TokenStream.cpp")

file(
  GLOB
  DLXLIB_HEADERS
  #
  "include/DLX/FloatRegister.hpp"
  "include/DLX/Instruction.hpp"
  "include/DLX/InstructionArgument.hpp"
  "include/DLX/InstructionImplementation.hpp"
  "include/DLX/InstructionInfo.hpp"
  "include/DLX/InstructionLibrary.hpp"
  "include/DLX/IntRegister.hpp"
  "include/DLX/Logger.hpp"
  "include/DLX/MemoryBlock.hpp"
  "include/DLX/OpCode.hpp"
  "include/DLX/ParsedProgram.hpp"
  "include/DLX/ParseError.hpp"
  "include/DLX/Parser.hpp"
  "include/DLX/ParserUtils.hpp"
  "include/DLX/Processor.hpp"
  "include/DLX/RegisterNames.hpp"
  "include/DLX/StatusRegister.hpp"
  "include/DLX/Token.hpp"
  "include/DLX/Tokenize.hpp"
  "include/DLX/TokenStream.hpp")

add_library(${PROJECT_NAME} STATIC ${DLXLIB_SOURCES} ${DLXLIB_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Phi::Core magic_enum::magic_enum spdlog::spdlog
  PRIVATE Phi::ProjectOptions)
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:RELWITHDBGINFO>:PHI_DEBUG>")
# We don't want a default logger
phi_enable_static_analyzers(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

if (${DLXEMU_COVERAGE_BUILD})
  target_compile_definitions(${PROJECT_NAME} PUBLIC "DLXEMU_COVERAGE_BUILD")
endif()
