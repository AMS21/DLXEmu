#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

dlx::InstructionLibrary lib;
dlx::Processor          processor;

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    std::string_view source = std::string_view(reinterpret_cast<const char*>(data), size);

    // Parse it
    dlx::ParsedProgram program = dlx::Parser::Parse(lib, source);

    // Run it
    processor.LoadProgram(program);
    processor.ExecuteCurrentProgram();

    return 0;
}
