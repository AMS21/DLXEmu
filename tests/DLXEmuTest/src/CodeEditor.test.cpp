#include <catch2/catch_test_macros.hpp>

#include <DLXEmu/CodeEditor.hpp>
#include <DLXEmu/Emulator.hpp>
#include <Phi/Config/Warning.hpp>
#include <Phi/Core/Log.hpp>
#include <imgui.h>
#include <imgui_internal.h>

TEST_CASE("CodeEditor")
{
    dlxemu::Emulator emulator;

    SECTION("Set/GetPalette")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Default
        dlxemu::CodeEditor::Palette dark_palette    = dlxemu::CodeEditor::GetDarkPalette();
        dlxemu::CodeEditor::Palette current_palette = editor.GetPalette();

        for (std::size_t i{0}; i < static_cast<std::size_t>(dlxemu::CodeEditor::PaletteIndex::Max);
             ++i)
        {
            CHECK(current_palette[i] == dark_palette[i]);
        }

        // SetPalette
        editor.SetPalette(dlxemu::CodeEditor::GetRetroBluePalette());
        dlxemu::CodeEditor::Palette retro_blue_palette = dlxemu::CodeEditor::GetRetroBluePalette();
        current_palette                                = editor.GetPalette();

        for (std::size_t i{0}; i < static_cast<std::size_t>(dlxemu::CodeEditor::PaletteIndex::Max);
             ++i)
        {
            CHECK(current_palette[i] == retro_blue_palette[i]);
        }
    }

    SECTION("ErrorMarkers")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Default
        dlxemu::CodeEditor::ErrorMarkers& markers = editor.GetErrorMarkers();
        CHECK(markers.empty());

        // SetErrorMarkers
        dlxemu::CodeEditor::ErrorMarkers test_markers;
        test_markers[0]  = "Test";
        test_markers[3]  = "Coolio";
        test_markers[12] = "Hey";
        test_markers[21] = "Awesome";

        editor.SetErrorMarkers(test_markers);
        markers = editor.GetErrorMarkers();

        CHECK(markers.size() == 4);
        CHECK(markers[0] == "Test");
        CHECK(markers[3] == "Coolio");
        CHECK(markers[12] == "Hey");
        CHECK(markers[21] == "Awesome");

        editor.ClearErrorMarkers();

        // AddErrorMarker
        editor.AddErrorMarker(0, "Hey");
        editor.AddErrorMarker(0, "Test");
        editor.AddErrorMarker(2, "Coolio");

        markers = editor.GetErrorMarkers();

        CHECK(markers.size() == 2);
        CHECK(markers[0] == "Hey\nTest");
        CHECK(markers[2] == "Coolio");
    }

    SECTION("Breakpoints")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Default
        dlxemu::CodeEditor::Breakpoints& break_points = editor.GetBreakpoints();
        CHECK(break_points.empty());

        // SetBreakPoints
        dlxemu::CodeEditor::Breakpoints test;
        test.insert(0);
        test.insert(3);
        editor.SetBreakpoints(test);

        break_points = editor.GetBreakpoints();
        CHECK(break_points.size() == 2);
        CHECK(break_points.contains(0));
        CHECK(break_points.contains(3));

        // ClearBreakpoints
        editor.ClearBreakPoints();

        CHECK(break_points.empty());

        // AddBreakpoint
        CHECK(editor.AddBreakpoint(3));
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        CHECK_FALSE(editor.AddBreakpoint(3));
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        CHECK(editor.AddBreakpoint(5));
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 2);
        CHECK(break_points.contains(3));
        CHECK(break_points.contains(5));

        // RemoveBreakpoint
        CHECK(editor.RemoveBreakpoint(5));
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        CHECK_FALSE(editor.RemoveBreakpoint(5));
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        // ToggleBreakpoint
        CHECK(editor.ToggleBreakpoint(5));
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 2);
        CHECK(break_points.contains(3));
        CHECK(break_points.contains(5));

        CHECK_FALSE(editor.ToggleBreakpoint(5));
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));
    }

    SECTION("Get/SetText")
    {
        dlxemu::CodeEditor editor{&emulator};

        std::string              text  = editor.GetText();
        std::vector<std::string> lines = editor.GetTextLines();
        CHECK(text.empty());
        CHECK(lines.size() == 1);
        CHECK(lines.at(0).empty());
        CHECK(editor.GetTotalLines() == 1);

        constexpr const char* t1{"A simple line"};
        editor.SetText(t1);
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t1);
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == t1);
        CHECK(editor.GetTotalLines() == 1);

        constexpr const char* t2{"\n"};
        editor.SetText(t2);
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t2);
        CHECK(lines.size() == 2);
        CHECK(lines.at(0).empty());
        CHECK(lines.at(1).empty());
        CHECK(editor.GetTotalLines() == 2);

        constexpr const char* t3{"\n\n"};
        editor.SetText(t3);
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t3);
        CHECK(lines.size() == 3);
        CHECK(lines.at(0).empty());
        CHECK(lines.at(1).empty());
        CHECK(lines.at(2).empty());
        CHECK(editor.GetTotalLines() == 3);

        constexpr const char* t4{"Multiple\nLines"};
        editor.SetText(t4);
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t4);
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "Multiple");
        CHECK(lines.at(1) == "Lines");
        CHECK(editor.GetTotalLines() == 2);

        constexpr const char* t5{"Line\nWith\nNewLines\n"};
        editor.SetText(t5);
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t5);
        CHECK(lines.size() == 4);
        CHECK(lines.at(0) == "Line");
        CHECK(lines.at(1) == "With");
        CHECK(lines.at(2) == "NewLines");
        CHECK(lines.at(3).empty());
        CHECK(editor.GetTotalLines() == 4);
    }

    SECTION("SetTextLines")
    {
        // TODO:
    }

    SECTION("InsertText")
    {
        // TODO:
    }

    SECTION("Overwrite")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK_FALSE(editor.IsOverwrite());

        editor.SetOverwrite(true);

        CHECK(editor.IsOverwrite());

        editor.SetOverwrite(false);

        CHECK_FALSE(editor.IsOverwrite());
    }

    SECTION("ReadOnly")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK_FALSE(editor.IsReadOnly());

        editor.SetReadOnly(true);

        CHECK(editor.IsReadOnly());

        editor.SetReadOnly(false);

        CHECK_FALSE(editor.IsReadOnly());
    }

    SECTION("ShowWhitespaces")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK_FALSE(editor.IsShowingWhitespaces());

        editor.SetShowWhitespaces(true);

        CHECK(editor.IsShowingWhitespaces());

        editor.SetShowWhitespaces(false);

        CHECK_FALSE(editor.IsShowingWhitespaces());
    }

    SECTION("TabSize")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK(editor.GetTabSize() == 4);

        editor.SetTabSize(1);
        CHECK(editor.GetTabSize() == 1);

        editor.SetTabSize(5);
        CHECK(editor.GetTabSize() == 5);

        // Clamped properly
        editor.SetTabSize(0);
        CHECK(editor.GetTabSize() == 1);

        editor.SetTabSize(100);
        CHECK(editor.GetTabSize() == 32);
    }

    SECTION("SetCursorPosition")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();

        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetText("123456789");
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0, 5));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 5);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 9);

        editor.InsertText("\n123456789\n123456789");
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 9);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 7));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 7);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 9);

        editor.SetText("");
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);
    }

    SECTION("MoveUp")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.MoveUp(1);
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.MoveUp(100);
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetText("Line 1\nLine 2\nLine 3\nLine 4\n");
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 4);
        CHECK(pos.m_Column == 0);

        editor.MoveUp(1);
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 3);
        CHECK(pos.m_Column == 0);

        editor.MoveUp(2);
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(3, 6));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 3);
        CHECK(pos.m_Column == 6);

        editor.MoveUp(1);
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 6);

        editor.MoveUp(2);
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 6);

        editor.SetText("short line\nA very long line of text");
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 100));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 24);

        editor.MoveUp();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 10);

        editor.SetText("This is quite a long line of text wow\ncute\nAnother long line of text");
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(2, 25));
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 25);

        editor.MoveUp();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 4);

        editor.MoveUp();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 25);

        editor.MoveUp();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 4));
        editor.MoveUp(2);
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);
    }
}

TEST_CASE("CodeEditor bad calls")
{
    dlxemu::Emulator emulator;

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

TEST_CASE("CodeEditor crashes")
{
    dlxemu::Emulator emulator;

    SECTION("crash-6ededd1eef55e21130e51a28a22b1275a0929cfd")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText("\n\n\n");
        editor.SetSelection(dlxemu::CodeEditor::Coordinates(0, 1993065),
                            dlxemu::CodeEditor::Coordinates(31, 1761607680));
        editor.Delete();
        editor.Undo(24);
        editor.Delete();

        editor.VerifyInternalState();
    }

    SECTION("Crash-1c525126120b9931b78d5b724f6338435e211037")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\n");
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0, 0));
        editor.Delete();
        editor.SetSelectionStart(dlxemu::CodeEditor::Coordinates(0, 30));
        editor.Delete();

        editor.VerifyInternalState();
    }

    SECTION("Crash-a37f577acccdcbfa8bdc8f53a570e1c6385c13da")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.SetText("z`3!\n");
        editor.InsertText("\x1E");
        editor.MoveBottom(true);
        editor.Delete();

        editor.VerifyInternalState();
    }

    SECTION("crash-2b9e8952b4d9676e2af93db7032ebca1dc2a9480")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.SetText("!");
        editor.SelectAll();
        editor.Delete();

        editor.VerifyInternalState();
    }

    SECTION("crash-4161f8892d023e82832c668012743711e7e8c263")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\x02\x01");
        editor.MoveHome(true);
        editor.InsertText("\n");

        editor.VerifyInternalState();
    }

    SECTION("crash-9caa85410b9d43f4c105d38ab169f0540d159648")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\x02\x01");
        editor.MoveHome(true);
        editor.InsertText("\n\n");

        editor.VerifyInternalState();
    }

    SECTION("crash-0c744fcdb9b8193836417ce839daa3174ce89e16")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.SetText("U");
        editor.SetSelection(dlxemu::CodeEditor::Coordinates(7, 1537),
                            dlxemu::CodeEditor::Coordinates(738197504, 30));
        editor.Delete();

        editor.VerifyInternalState();
    }

    SECTION("crash-4620fed3f283876c8534a78e77bbb319a9def029")
    {
        dlxemu::CodeEditor editor(&emulator);

        std::vector<std::string> vec;
        vec.reserve(7);

        vec.emplace_back("");
        vec.emplace_back("");
        vec.emplace_back("");
        vec.emplace_back("");
        vec.emplace_back("");
        vec.emplace_back("");
        vec.emplace_back("\x1E");

        REQUIRE(vec.size() == 7);

        editor.SetTextLines(vec);
        editor.SetSelection(dlxemu::CodeEditor::Coordinates(0, 30),
                            dlxemu::CodeEditor::Coordinates(30, 2883584));
        editor.Delete();

        editor.VerifyInternalState();
    }

    SECTION("crash-73ef47764c46d77f157ef9399720189dbbeaeee3")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("(#8(\t");
        editor.MoveBottom(true);
        editor.Delete(); // Instead of cut

        editor.VerifyInternalState();
    }

    SECTION("crash-ebbfccfff485022666d0448d53c7634d31f98c9a")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\t\x44\x4D");
        editor.MoveEnd(true);
        editor.Delete();

        editor.VerifyInternalState();
    }

    SECTION("crash-aeb78eb087c7e15d3bc53666d21575ec7b73bd02")
    {
        ImGuiContext* ctx = ImGui::CreateContext();
        REQUIRE(ctx);

        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("(#8(\x7F\t\x07");
        editor.Copy();
        editor.Paste();
        editor.Undo(638844961);

        editor.VerifyInternalState();

        ImGui::DestroyContext(ctx);
    }

    SECTION("crash-1bc6fd5daba7cdfcacbc166f238326b0b3ed7b1e")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\tDM+");
        editor.Delete();
        editor.MoveBottom(true);
        editor.Delete();

        editor.VerifyInternalState();
    }
}