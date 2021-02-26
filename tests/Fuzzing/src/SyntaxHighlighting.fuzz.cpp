#include <cstddef>
#include <cstdint>
#include <string_view>

// Evil hack to access private members and functions
#define private public
#include <DLXEmu/Emulator.hpp>

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    static dlxemu::Emulator emu;

    std::string_view source = std::string_view(reinterpret_cast<const char*>(data), size);

    dlxemu::CodeEditor editor = emu.m_CodeEditor;

    // Parse it
    editor.SetText(std::string(source.data(), source.size()));
    editor.m_FullText = editor.GetText();

    emu.ParseProgram(editor.m_FullText);

    editor.ColorizeInternal();

    return 0;
}
