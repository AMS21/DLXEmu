#include <DLX/Parser.hpp>
#include <cstddef>
#include <cstdint>
#include <string>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    std::string_view source = std::string_view(reinterpret_cast<const char*>(data), size);

    // Parse it
    dlx::Parser::Parse(source);

    return 0;
}
