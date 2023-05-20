#include "StructureParser.hpp"
#include <DLXEmu/Emulator.hpp>
#include <phi/compiler_support/warning.hpp>
#include <cstddef>

PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const phi::uint8_t* data, phi::size_t size)
{
    static dlxemu::Emulator emu;

    std::string source = fuzz::ParseAsStrucutedDLXCode(data, size);

    dlxemu::CodeEditor& editor = emu.GetEditor();

    // Parse it
    editor.SetText(source);
    editor.m_FullText = editor.GetText();

    emu.ParseProgram(editor.m_FullText);

    editor.ColorizeInternal();

    return 0;
}
