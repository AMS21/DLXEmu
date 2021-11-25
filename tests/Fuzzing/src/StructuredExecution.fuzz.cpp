#include "StructureParser.hpp"
#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <cstddef>
#include <cstdint>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    static dlx::Processor processor;

    std::string source = fuzz::ParseAsStrucutedDLXCode(data, size);

    // Parse it
    dlx::ParsedProgram program = dlx::Parser::Parse(source);

    // Clear registers and memory
    processor.ClearRegisters();
    processor.ClearMemory();

    // Execute
    if (processor.LoadProgram(program))
    {
        processor.ExecuteCurrentProgram();
    }

    return 0;
}
