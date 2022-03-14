#include <catch2/catch_test_macros.hpp>

#include <DLXEmu/CodeEditor.hpp>
#include <DLXEmu/Emulator.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>

void BeginImGui()
{
    ImGuiContext* ctx = ImGui::CreateContext();
    REQUIRE(ctx);

    ImGuiIO& io = ImGui::GetIO();

    // Enforce valid display size
    io.DisplaySize.x = 1024.0f;
    io.DisplaySize.y = 768.0f;

    // Enfore valid DeltaTime
    io.DeltaTime = 1.0f / 60.0f;

    // Enforce valid space key mapping
    io.KeyMap[ImGuiKey_Space] = 0;

    // Don't save any config
    io.IniFilename = nullptr;

    // Build atlas
    unsigned char* tex_pixels{nullptr};
    int            tex_w;
    int            tex_h;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

    ImGui::NewFrame();
}

void EndImgui()
{
    ImGui::EndFrame();

    ImGui::DestroyContext(ImGui::GetCurrentContext());
}

// TODO: MoveX with 0 amount being noop

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

        // SetPalette - Dark
        editor.SetPalette(dlxemu::CodeEditor::GetDarkPalette());
        editor.VerifyInternalState();

        current_palette = editor.GetPalette();

        for (std::size_t i{0u}; i < static_cast<std::size_t>(dlxemu::CodeEditor::PaletteIndex::Max);
             ++i)
        {
            CHECK(current_palette[i] == dark_palette[i]);
        }

        // SetPalette - Light
        editor.SetPalette(dlxemu::CodeEditor::GetLightPalette());
        editor.VerifyInternalState();

        dlxemu::CodeEditor::Palette light_palette = dlxemu::CodeEditor::GetLightPalette();
        current_palette                           = editor.GetPalette();

        for (std::size_t i{0u}; i < static_cast<std::size_t>(dlxemu::CodeEditor::PaletteIndex::Max);
             ++i)
        {
            CHECK(current_palette[i] == light_palette[i]);
        }

        // SetPalette - Retro Blue
        editor.SetPalette(dlxemu::CodeEditor::GetRetroBluePalette());
        editor.VerifyInternalState();

        dlxemu::CodeEditor::Palette retro_blue_palette = dlxemu::CodeEditor::GetRetroBluePalette();
        current_palette                                = editor.GetPalette();

        for (std::size_t i{0u}; i < static_cast<std::size_t>(dlxemu::CodeEditor::PaletteIndex::Max);
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
        editor.VerifyInternalState();
        markers = editor.GetErrorMarkers();

        CHECK(markers.size() == 4);
        CHECK(markers[0] == "Test");
        CHECK(markers[3] == "Coolio");
        CHECK(markers[12] == "Hey");
        CHECK(markers[21] == "Awesome");

        editor.ClearErrorMarkers();
        editor.VerifyInternalState();

        // AddErrorMarker
        editor.AddErrorMarker(0, "Hey");
        editor.AddErrorMarker(0, "Test");
        editor.AddErrorMarker(2, "Coolio");

        markers = editor.GetErrorMarkers();

        CHECK(markers.size() == 2);
        CHECK(markers[0] == "Hey\nTest");
        CHECK(markers[2] == "Coolio");

        // const
        const dlxemu::CodeEditor const_editor{&emulator};

        const dlxemu::CodeEditor::ErrorMarkers& const_markers = const_editor.GetErrorMarkers();
        CHECK(const_markers.empty());
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
        editor.VerifyInternalState();

        break_points = editor.GetBreakpoints();
        CHECK(break_points.size() == 2);
        CHECK(break_points.contains(0));
        CHECK(break_points.contains(3));

        // ClearBreakpoints
        editor.ClearBreakPoints();
        editor.VerifyInternalState();

        CHECK(break_points.empty());

        // AddBreakpoint
        CHECK(editor.AddBreakpoint(3));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        CHECK_FALSE(editor.AddBreakpoint(3));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        CHECK(editor.AddBreakpoint(5));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 2);
        CHECK(break_points.contains(3));
        CHECK(break_points.contains(5));

        // RemoveBreakpoint
        CHECK(editor.RemoveBreakpoint(5));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        CHECK_FALSE(editor.RemoveBreakpoint(5));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        // ToggleBreakpoint
        CHECK(editor.ToggleBreakpoint(5));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 2);
        CHECK(break_points.contains(3));
        CHECK(break_points.contains(5));

        CHECK_FALSE(editor.ToggleBreakpoint(5));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3));

        // const
        const dlxemu::CodeEditor const_editor{&emulator};

        const dlxemu::CodeEditor::Breakpoints& const_break_points = const_editor.GetBreakpoints();
        CHECK(const_break_points.empty());
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
        editor.VerifyInternalState();
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t1);
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == t1);
        CHECK(editor.GetTotalLines() == 1);

        constexpr const char* t2{"\n"};
        editor.SetText(t2);
        editor.VerifyInternalState();
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t2);
        CHECK(lines.size() == 2);
        CHECK(lines.at(0).empty());
        CHECK(lines.at(1).empty());
        CHECK(editor.GetTotalLines() == 2);

        constexpr const char* t3{"\n\n"};
        editor.SetText(t3);
        editor.VerifyInternalState();
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
        editor.VerifyInternalState();
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t4);
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "Multiple");
        CHECK(lines.at(1) == "Lines");
        CHECK(editor.GetTotalLines() == 2);

        constexpr const char* t5{"Line\nWith\nNewLines\n"};
        editor.SetText(t5);
        editor.VerifyInternalState();
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
        dlxemu::CodeEditor editor{&emulator};

        std::vector<std::string> new_lines;
        new_lines.reserve(2u);
        new_lines.emplace_back("Hello");
        new_lines.emplace_back("World");

        editor.SetTextLines(new_lines);
        editor.VerifyInternalState();

        std::string              text  = editor.GetText();
        std::vector<std::string> lines = editor.GetTextLines();
        CHECK(text == "Hello\nWorld");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "Hello");
        CHECK(lines.at(1) == "World");
        CHECK(editor.GetTotalLines() == 2);

        new_lines.clear();
        new_lines.reserve(0);

        editor.SetTextLines(new_lines);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text.empty());
        CHECK(lines.size() == 1);
        CHECK(lines.at(0).empty());
        CHECK(editor.GetTotalLines() == 1);
    }

    SECTION("InsertText")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText("Hello World!");
        editor.VerifyInternalState();

        std::string              text  = editor.GetText();
        std::vector<std::string> lines = editor.GetTextLines();
        CHECK(text == "Hello World!");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "Hello World!");
        CHECK(editor.GetTotalLines() == 1);

        editor.InsertText("\nTest string\n");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Hello World!\nTest string\n");
        CHECK(lines.size() == 3);
        CHECK(lines.at(0) == "Hello World!");
        CHECK(lines.at(1) == "Test string");
        CHECK(lines.at(2).empty());
        CHECK(editor.GetTotalLines() == 3);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0, 0));
        editor.VerifyInternalState();
        editor.InsertText("I say hi and ");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "I say hi and Hello World!\nTest string\n");
        CHECK(lines.size() == 3);
        CHECK(lines.at(0) == "I say hi and Hello World!");
        CHECK(lines.at(1) == "Test string");
        CHECK(lines.at(2).empty());
        CHECK(editor.GetTotalLines() == 3);

        editor.SelectAll();
        editor.VerifyInternalState();
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 4));
        editor.VerifyInternalState();
        editor.InsertText(" awesome");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "I say hi and Hello World!\nTest awesome string\n");
        CHECK(lines.size() == 3);
        CHECK(lines.at(0) == "I say hi and Hello World!");
        CHECK(lines.at(1) == "Test awesome string");
        CHECK(lines.at(2).empty());
        CHECK(editor.GetTotalLines() == 3);

        // Insert nullptr
        editor.InsertText(nullptr);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "I say hi and Hello World!\nTest awesome string\n");
        CHECK(lines.size() == 3);
        CHECK(lines.at(0) == "I say hi and Hello World!");
        CHECK(lines.at(1) == "Test awesome string");
        CHECK(lines.at(2).empty());
        CHECK(editor.GetTotalLines() == 3);

        // Insert empty string
        editor.InsertText("");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "I say hi and Hello World!\nTest awesome string\n");
        CHECK(lines.size() == 3);
        CHECK(lines.at(0) == "I say hi and Hello World!");
        CHECK(lines.at(1) == "Test awesome string");
        CHECK(lines.at(2).empty());
        CHECK(editor.GetTotalLines() == 3);

        // Insert while in read-only mode
        editor.SetReadOnly(true);
        editor.VerifyInternalState();
        editor.InsertText("This should be a no-op");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "I say hi and Hello World!\nTest awesome string\n");
        CHECK(lines.size() == 3);
        CHECK(lines.at(0) == "I say hi and Hello World!");
        CHECK(lines.at(1) == "Test awesome string");
        CHECK(lines.at(2).empty());
        CHECK(editor.GetTotalLines() == 3);
    }

    SECTION("EnterCharacter")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.EnterCharacter('H');
        editor.VerifyInternalState();

        std::string              text  = editor.GetText();
        std::vector<std::string> lines = editor.GetTextLines();
        CHECK(text == "H");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "H");
        CHECK(editor.GetTotalLines() == 1);

        editor.EnterCharacter('i');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Hi");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "Hi");
        CHECK(editor.GetTotalLines() == 1);

        editor.MoveHome();
        editor.VerifyInternalState();
        editor.EnterCharacter('-');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "-Hi");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "-Hi");
        CHECK(editor.GetTotalLines() == 1);

        editor.SelectAll();
        editor.VerifyInternalState();
        editor.EnterCharacter('A');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "A");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "A");
        CHECK(editor.GetTotalLines() == 1);

        editor.EnterCharacter('\n');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "A\n");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "A");
        CHECK(lines.at(1).empty());
        CHECK(editor.GetTotalLines() == 2);

        editor.SetOverwrite(true);
        editor.VerifyInternalState();
        editor.MoveUp(31000);
        editor.VerifyInternalState();
        editor.EnterCharacter('B');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "B\n");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "B");
        CHECK(lines.at(1).empty());
        CHECK(editor.GetTotalLines() == 2);

        editor.EnterCharacter('C');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "BC\n");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "BC");
        CHECK(lines.at(1).empty());
        CHECK(editor.GetTotalLines() == 2);

        editor.SetOverwrite(false);
        editor.VerifyInternalState();
        editor.SetReadOnly(true);
        editor.VerifyInternalState();
        editor.EnterCharacter('D');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "BC\n");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "BC");
        CHECK(lines.at(1).empty());
        CHECK(editor.GetTotalLines() == 2);
    }

    SECTION("Overwrite")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK_FALSE(editor.IsOverwrite());

        editor.SetOverwrite(true);
        editor.VerifyInternalState();

        CHECK(editor.IsOverwrite());

        editor.SetOverwrite(false);
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsOverwrite());
    }

    SECTION("ReadOnly")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK_FALSE(editor.IsReadOnly());

        editor.SetReadOnly(true);
        editor.VerifyInternalState();

        CHECK(editor.IsReadOnly());

        editor.SetReadOnly(false);
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsReadOnly());
    }

    SECTION("ShowWhitespaces")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK_FALSE(editor.IsShowingWhitespaces());

        editor.SetShowWhitespaces(true);
        editor.VerifyInternalState();

        CHECK(editor.IsShowingWhitespaces());

        editor.SetShowWhitespaces(false);
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsShowingWhitespaces());
    }

    SECTION("Colorizer")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Default
        CHECK(editor.IsColorizerEnabled());

        editor.SetColorizerEnable(false);
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsColorizerEnabled());

        editor.SetColorizerEnable(true);
        editor.VerifyInternalState();

        CHECK(editor.IsColorizerEnabled());
    }

    SECTION("TabSize")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK(editor.GetTabSize() == 4);

        editor.SetTabSize(1);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize() == 1);

        editor.SetTabSize(5);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize() == 5);

        // Clamped properly
        editor.SetTabSize(0);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize() == 1);

        editor.SetTabSize(100);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize() == 32);
    }

    SECTION("SetCursorPosition")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();

        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetText("123456789");
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0, 5));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 5);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 9);

        editor.InsertText("\n123456789\n123456789");
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 9);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 7));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 7);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 9);

        editor.SetText("");
        editor.VerifyInternalState();
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
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.MoveUp(100);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetText("Line 1\nLine 2\nLine 3\nLine 4\n");
        editor.VerifyInternalState();
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 4);
        CHECK(pos.m_Column == 0);

        editor.MoveUp(1);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 3);
        CHECK(pos.m_Column == 0);

        editor.MoveUp(2);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(3, 6));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 3);
        CHECK(pos.m_Column == 6);

        editor.MoveUp(1);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 6);

        editor.MoveUp(2);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 6);

        editor.SetText("short line\nA very long line of text");
        editor.VerifyInternalState();
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 24);

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 10);

        editor.SetText("This is quite a long line of text wow\ncute\nAnother long line of text");
        editor.VerifyInternalState();
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(2, 25));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 25);

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 4);

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 25);

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 4));
        editor.VerifyInternalState();
        editor.MoveUp(2);
        editor.VerifyInternalState();
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
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart column negative")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = 0;
        coords.m_Column = -1;

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart both negative")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = -1;
        coords.m_Column = -1;

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart line very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = std::numeric_limits<std::int32_t>::max();
        coords.m_Column = 0;

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart column very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = 0;
        coords.m_Column = std::numeric_limits<std::int32_t>::max();

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart both very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = std::numeric_limits<std::int32_t>::max();
        coords.m_Column = std::numeric_limits<std::int32_t>::max();

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetCursorPosition invalid")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = -1;
        coords.m_Column = -1;

        editor.SetCursorPosition(coords);
        editor.VerifyInternalState();

        volatile std::string val = editor.GetCurrentLineText();
        PHI_UNUSED_VARIABLE(val);
        editor.VerifyInternalState();
    }

    SECTION("Delete invalid pos")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = -1;
        coords.m_Column = 10;

        editor.SetCursorPosition(coords);
        editor.VerifyInternalState();
        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("SetText and delete with control character")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.SetText("\x1E");
        editor.VerifyInternalState();

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = 0;
        coords.m_Column = 10;

        editor.SetCursorPosition(coords);
        editor.VerifyInternalState();
        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("InsertText and delete")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText(";;");
        editor.VerifyInternalState();

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = 0;
        coords.m_Column = 7935;

        editor.SetCursorPosition(coords);
        editor.VerifyInternalState();
        editor.Delete();
        editor.VerifyInternalState();
    }
}

TEST_CASE("CodeEditor crashes")
{
    dlxemu::Emulator emulator;

    SECTION("crash-6ededd1eef55e21130e51a28a22b1275a0929cfd")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText("\n\n\n");
        editor.VerifyInternalState();

        editor.SetSelection(dlxemu::CodeEditor::Coordinates(0, 1993065),
                            dlxemu::CodeEditor::Coordinates(31, 1761607680));
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();

        editor.Undo(24);
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("Crash-1c525126120b9931b78d5b724f6338435e211037")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\n");
        editor.VerifyInternalState();

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0, 0));
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();

        editor.SetSelectionStart(dlxemu::CodeEditor::Coordinates(0, 30));
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("Crash-a37f577acccdcbfa8bdc8f53a570e1c6385c13da")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.SetText("z`3!\n");
        editor.VerifyInternalState();

        editor.InsertText("\x1E");
        editor.VerifyInternalState();

        editor.MoveBottom(true);
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("crash-2b9e8952b4d9676e2af93db7032ebca1dc2a9480")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.SetText("!");
        editor.VerifyInternalState();

        editor.SelectAll();
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("crash-4161f8892d023e82832c668012743711e7e8c263")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\x02\x01");
        editor.VerifyInternalState();

        editor.MoveHome(true);
        editor.VerifyInternalState();

        editor.InsertText("\n");
        editor.VerifyInternalState();
    }

    SECTION("crash-9caa85410b9d43f4c105d38ab169f0540d159648")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\x02\x01");
        editor.VerifyInternalState();

        editor.MoveHome(true);
        editor.VerifyInternalState();

        editor.InsertText("\n\n");
        editor.VerifyInternalState();
    }

    SECTION("crash-0c744fcdb9b8193836417ce839daa3174ce89e16")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.SetText("U");
        editor.VerifyInternalState();

        editor.SetSelection(dlxemu::CodeEditor::Coordinates(7, 1537),
                            dlxemu::CodeEditor::Coordinates(738197504, 30));
        editor.VerifyInternalState();

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
        editor.VerifyInternalState();

        editor.SetSelection(dlxemu::CodeEditor::Coordinates(0, 30),
                            dlxemu::CodeEditor::Coordinates(30, 2883584));
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("crash-73ef47764c46d77f157ef9399720189dbbeaeee3")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("(#8(\t");
        editor.VerifyInternalState();

        editor.MoveBottom(true);
        editor.VerifyInternalState();

        editor.Delete(); // Instead of cut
        editor.VerifyInternalState();
    }

    SECTION("crash-ebbfccfff485022666d0448d53c7634d31f98c9a")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\t\x44\x4D");
        editor.VerifyInternalState();

        editor.MoveEnd(true);
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("crash-aeb78eb087c7e15d3bc53666d21575ec7b73bd02")
    {
        ImGuiContext* ctx = ImGui::CreateContext();
        REQUIRE(ctx);

        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("(#8(\x7F\t\x07");
        editor.VerifyInternalState();

        editor.Copy();
        editor.VerifyInternalState();

        editor.Paste();
        editor.VerifyInternalState();

        editor.Undo(638844961);
        editor.VerifyInternalState();

        ImGui::DestroyContext(ctx);
    }

    SECTION("crash-1bc6fd5daba7cdfcacbc166f238326b0b3ed7b1e")
    {
        dlxemu::CodeEditor editor(&emulator);

        editor.InsertText("\tDM+");
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();

        editor.MoveBottom(true);
        editor.VerifyInternalState();

        editor.Delete();
        editor.VerifyInternalState();
    }

    SECTION("crash-28853252177dc5b6be74f8247bde0d2a2b4f87b5")
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText("kA`\"#;#");
        editor.VerifyInternalState();

        CHECK(editor.GetText().size() == 7);
        CHECK(editor.GetTotalLines() == 1);

        editor.Render({0.0f, 0.0f}, true);
        editor.VerifyInternalState();

        EndImgui();
    }

    SECTION("crash-c567e237f4822cff4cab65198f9ea3b393e6f92c")
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        editor.SetText(" ");
        editor.VerifyInternalState();

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.InsertText(":x;(");
        editor.VerifyInternalState();

        editor.Render({0.0f, 0.0f}, true);
        editor.VerifyInternalState();

        EndImgui();
    }

    SECTION("crash-1e4a2c5c4b7bd8fe934c1eb3b5e0e98ed3474b72")
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wconstant-conversion")
        PHI_GCC_SUPPRESS_WARNING_PUSH()
        PHI_GCC_SUPPRESS_WARNING("-Wmultichar")
        PHI_GCC_SUPPRESS_WARNING("-Woverflow")

        editor.EnterCharacter('\0xFF', true);
        editor.VerifyInternalState();

        PHI_GCC_SUPPRESS_WARNING_POP()
        PHI_CLANG_SUPPRESS_WARNING_POP()

        editor.EnterCharacter('\n', true);
        editor.VerifyInternalState();

        editor.InsertText("(m:M:x;");
        editor.VerifyInternalState();

        editor.Render({0.0f, 0.0f}, true);
        editor.VerifyInternalState();

        EndImgui();
    }

    SECTION("Crash-b969d74f5fc10237a879950cd37541614ee459e4")
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        editor.InsertText("A\tJ");
        editor.VerifyInternalState();

        editor.Render({0.0, 0.0}, true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(0, "");
        editor.VerifyInternalState();

        editor.AddErrorMarker(0, "");
        editor.VerifyInternalState();

        editor.MoveRight(0, true, true);
        editor.VerifyInternalState();

        editor.AddErrorMarker(0, "");
        editor.VerifyInternalState();

        EndImgui();
    }

    SECTION("crash-4e00b6223382d32d373d6d47d46d844a422c77a8")
    {
        // Crash
        {
            BeginImGui();

            dlxemu::CodeEditor editor{&emulator};

            editor.InsertText(" \tJ");
            editor.VerifyInternalState();

            editor.Render({0.0, 0.0}, true);
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "       ");
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "      ");
            editor.VerifyInternalState();

            editor.MoveRight(0, true, true);
            editor.VerifyInternalState();

            EndImgui();
        }

        // MoveLeft
        {
            BeginImGui();

            dlxemu::CodeEditor editor{&emulator};

            editor.InsertText(" \tJ");
            editor.VerifyInternalState();

            editor.Render({0.0, 0.0}, true);
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "       ");
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "      ");
            editor.VerifyInternalState();

            editor.MoveLeft(0, true, true);
            editor.VerifyInternalState();

            EndImgui();
        }

        // MoveUp
        {
            BeginImGui();

            dlxemu::CodeEditor editor{&emulator};

            editor.InsertText(" \tJ");
            editor.VerifyInternalState();

            editor.Render({0.0, 0.0}, true);
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "       ");
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "      ");
            editor.VerifyInternalState();

            editor.MoveUp(0, true);
            editor.VerifyInternalState();

            EndImgui();
        }

        // MoveDown
        {
            BeginImGui();

            dlxemu::CodeEditor editor{&emulator};

            editor.InsertText(" \tJ");
            editor.VerifyInternalState();

            editor.Render({0.0, 0.0}, true);
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "       ");
            editor.VerifyInternalState();

            editor.AddErrorMarker(538976288, "      ");
            editor.VerifyInternalState();

            editor.MoveDown(0, true);
            editor.VerifyInternalState();

            EndImgui();
        }
    }
}
