#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <DLX/RegisterNames.hpp>
#include <Phi/Core/Log.hpp>
#include <magic_enum.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

dlx::InstructionLibrary lib;
dlx::Processor          processor;

const char*           dummy_data        = "ADD R1 R2 R3";
constexpr std::size_t sizeof_dummy_data = 13;

extern "C" size_t LLVMFuzzerMutate(uint8_t* Data, size_t Size, size_t MaxSize);

extern "C" size_t LLVMFuzzerCustomMutator(uint8_t* data, size_t size, size_t max_size,
                                          unsigned int seed)
{
    std::string_view source = std::string_view(reinterpret_cast<const char*>(data), size);

    auto res = dlx::Parser::Parse(lib, source);
    if (!res.m_ParseErrors.empty() || res.m_Instructions.empty())
    {
        if (size >= sizeof_dummy_data)
        {
            std::strncpy(reinterpret_cast<char*>(data), dummy_data, sizeof_dummy_data);
            return sizeof_dummy_data;
        }
        else
        {
            if (size != 0)
            {
                std::strncpy(reinterpret_cast<char*>(data), "", 1);
            }

            return 0;
        }
    }

    std::size_t len = LLVMFuzzerMutate(data, size, max_size);

    std::string_view mutated_source = std::string_view(reinterpret_cast<const char*>(data), size);

    res = dlx::Parser::Parse(lib, mutated_source);
    if (!res.m_ParseErrors.empty() || res.m_Instructions.empty())
    {
        return 0;
    }

    return len;
}

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    std::string_view source = std::string_view(reinterpret_cast<const char*>(data), size);

    // Parse it
    dlx::ParsedProgram program = dlx::Parser::Parse(lib, source);

    // Clear registers and memory
    processor.ClearRegisters();
    processor.ClearMemory();

    // Execute
    processor.LoadProgram(program);
    processor.ExecuteCurrentProgram();

    return 0;
}
