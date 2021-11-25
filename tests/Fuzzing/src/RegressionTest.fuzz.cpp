#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    static dlx::Processor processor;

    std::string_view source = std::string_view(reinterpret_cast<const char*>(data), size);

    // Parse it
    dlx::ParsedProgram program = dlx::Parser::Parse(source);

    // Clear registers and memory
    processor.ClearRegisters();
    processor.ClearMemory();

    // Run it
    if (processor.LoadProgram(program))
    {
        processor.ExecuteCurrentProgram();
    }

    return 0;
}
