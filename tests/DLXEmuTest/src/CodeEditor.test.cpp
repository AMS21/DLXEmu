#include "DLXEmu/Emulator.hpp"
#include "Phi/Config/Warning.hpp"
#include <catch2/catch_test_macros.hpp>

#include <DLXEmu/CodeEditor.hpp>
#include <Phi/Core/Log.hpp>

TEST_CASE("CodeEditor")
{
    phi::Log::initialize_default_loggers();

    dlxemu::Emulator emulator;

    SECTION("Bad calls")
    {
        SECTION("SetSelectionStart line negative")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = -1;
            coords.m_Column = 0;

            editor.SetSelectionEnd(coords);
        }

        SECTION("SetSelectionStart column negative")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = 0;
            coords.m_Column = -1;

            editor.SetSelectionEnd(coords);
        }

        SECTION("SetSelectionStart both negative")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = -1;
            coords.m_Column = -1;

            editor.SetSelectionEnd(coords);
        }

        SECTION("SetSelectionStart line very high")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = std::numeric_limits<std::int32_t>::max();
            coords.m_Column = 0;

            editor.SetSelectionEnd(coords);
        }

        SECTION("SetSelectionStart column very high")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = 0;
            coords.m_Column = std::numeric_limits<std::int32_t>::max();

            editor.SetSelectionEnd(coords);
        }

        SECTION("SetSelectionStart both very high")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = std::numeric_limits<std::int32_t>::max();
            coords.m_Column = std::numeric_limits<std::int32_t>::max();

            editor.SetSelectionEnd(coords);
        }

        SECTION("SetCursorPosition invalid")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = -1;
            coords.m_Column = -1;

            editor.SetCursorPosition(coords);

            volatile std::string val = editor.GetCurrentLineText();
            PHI_UNUSED_VARIABLE(val);
        }

        SECTION("Delete invalid pos")
        {
            dlxemu::CodeEditor editor{&emulator};

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = -1;
            coords.m_Column = 10;

            editor.SetCursorPosition(coords);
            editor.Delete();
        }

        SECTION("SetText and delete with control character")
        {
            dlxemu::CodeEditor editor{&emulator};

            editor.SetText("\x1E");

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = 0;
            coords.m_Column = 10;

            editor.SetCursorPosition(coords);

            editor.Delete();
        }

        SECTION("InsertText and delete")
        {
            dlxemu::CodeEditor editor{&emulator};

            editor.InsertText(";;");

            dlxemu::CodeEditor::Coordinates coords;
            coords.m_Line   = 0;
            coords.m_Column = 7935;

            editor.SetCursorPosition(coords);

            editor.Delete();
        }
    }
}