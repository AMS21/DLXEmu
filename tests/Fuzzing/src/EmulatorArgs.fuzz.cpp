#include <DLXEmu/Emulator.hpp>
#include <limits>
#include <vector>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    // Only accept properly null terminated strings
    if (size < 1 || data[size - 1] != '\0')
    {
        return 1;
    }

    dlxemu::Emulator emulator;

    // Build argv
    std::vector<char*> argv;
    argv.reserve(10);

    std::size_t begin{0u};
    for (std::size_t index{0}; index < size; ++index)
    {
        char c = static_cast<char>(data[index]);

        if (c == '\0')
        {
            // Disallow empty strings
            if (begin == index)
            {
                return 1;
            }

            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
            argv.push_back(const_cast<char*>(reinterpret_cast<const char*>(data + begin)));
            begin = index + 1u;
        }
    }

    // Can't eccept too large buffers
    if (argv.size() > std::numeric_limits<std::int32_t>::max())
    {
        return 1;
    }

    (void)emulator.HandleCommandLineArguments(static_cast<std::int32_t>(argv.size()), argv.data());

    return 0;
}
