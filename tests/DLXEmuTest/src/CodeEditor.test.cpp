#include <catch2/catch_test_macros.hpp>

#include "SetupImGui.hpp"
#include <DLXEmu/CodeEditor.hpp>
#include <DLXEmu/Emulator.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>
#include <limits>

// TODO: MoveX with 0 amount being noop

TEST_CASE("CodeEditor::Coordinates")
{
    const dlxemu::CodeEditor::Coordinates default_coord{};
    CHECK(default_coord.m_Line == 0u);
    CHECK(default_coord.m_Column == 0u);

    const dlxemu::CodeEditor::Coordinates coords{42u, 22u};
    CHECK(coords.m_Line == 42u);
    CHECK(coords.m_Column == 22);

    const dlxemu::CodeEditor::Coordinates invalid{dlxemu::CodeEditor::Coordinates::Invalid()};
    CHECK(invalid.m_Line == -1);
    CHECK(invalid.m_Column == -1);

    const dlxemu::CodeEditor::Coordinates a{1u, 0u};
    const dlxemu::CodeEditor::Coordinates b{1u, 1u};
    const dlxemu::CodeEditor::Coordinates c{2u, 0u};
    const dlxemu::CodeEditor::Coordinates d{2u, 2u};
    const dlxemu::CodeEditor::Coordinates e{1u, 0u};

    SECTION("operator==")
    {
        CHECK(a == a);
        CHECK_FALSE(a == b);
        CHECK_FALSE(a == c);
        CHECK_FALSE(a == d);
        CHECK(a == e);

        CHECK_FALSE(b == a);
        CHECK(b == b);
        CHECK_FALSE(b == c);
        CHECK_FALSE(b == d);
        CHECK_FALSE(b == e);

        CHECK_FALSE(c == a);
        CHECK_FALSE(c == b);
        CHECK(c == c);
        CHECK_FALSE(c == d);
        CHECK_FALSE(c == e);

        CHECK_FALSE(d == a);
        CHECK_FALSE(d == b);
        CHECK_FALSE(d == c);
        CHECK(d == d);
        CHECK_FALSE(d == e);

        CHECK(e == a);
        CHECK_FALSE(e == b);
        CHECK_FALSE(e == c);
        CHECK_FALSE(e == d);
        CHECK(e == e);
    }

    SECTION("operator!=")
    {
        CHECK_FALSE(a != a);
        CHECK(a != b);
        CHECK(a != c);
        CHECK(a != d);
        CHECK_FALSE(a != e);

        CHECK(b != a);
        CHECK_FALSE(b != b);
        CHECK(b != c);
        CHECK(b != d);
        CHECK(b != e);

        CHECK(c != a);
        CHECK(c != b);
        CHECK_FALSE(c != c);
        CHECK(c != d);
        CHECK(c != e);

        CHECK(d != a);
        CHECK(d != b);
        CHECK(d != c);
        CHECK_FALSE(d != d);
        CHECK(d != e);

        CHECK_FALSE(e != a);
        CHECK(e != b);
        CHECK(e != c);
        CHECK(e != d);
        CHECK_FALSE(e != e);
    }

    SECTION("operator<")
    {
        CHECK_FALSE(a < a);
        CHECK(a < b);
        CHECK(a < c);
        CHECK(a < d);
        CHECK_FALSE(a < e);

        CHECK_FALSE(b < a);
        CHECK_FALSE(b < b);
        CHECK(b < c);
        CHECK(b < d);
        CHECK_FALSE(b < e);

        CHECK_FALSE(c < a);
        CHECK_FALSE(c < b);
        CHECK_FALSE(c < c);
        CHECK(c < d);
        CHECK_FALSE(c < e);

        CHECK_FALSE(d < a);
        CHECK_FALSE(d < b);
        CHECK_FALSE(d < c);
        CHECK_FALSE(d < d);
        CHECK_FALSE(d < e);

        CHECK_FALSE(e < a);
        CHECK(e < b);
        CHECK(e < c);
        CHECK(e < d);
        CHECK_FALSE(e < e);
    }

    SECTION("operator>")
    {
        CHECK_FALSE(a > a);
        CHECK_FALSE(a > b);
        CHECK_FALSE(a > c);
        CHECK_FALSE(a > d);
        CHECK_FALSE(a > e);

        CHECK(b > a);
        CHECK_FALSE(b > b);
        CHECK_FALSE(b > c);
        CHECK_FALSE(b > d);
        CHECK(b > e);

        CHECK(c > a);
        CHECK(c > b);
        CHECK_FALSE(c > c);
        CHECK_FALSE(c > d);
        CHECK(c > e);

        CHECK(d > a);
        CHECK(d > b);
        CHECK(d > c);
        CHECK_FALSE(d > d);
        CHECK(d > e);

        CHECK_FALSE(e > a);
        CHECK_FALSE(e > b);
        CHECK_FALSE(e > c);
        CHECK_FALSE(e > d);
        CHECK_FALSE(e > e);
    }

    SECTION("operator<=")
    {
        CHECK(a <= a);
        CHECK(a <= b);
        CHECK(a <= c);
        CHECK(a <= d);
        CHECK(a <= e);

        CHECK_FALSE(b <= a);
        CHECK(b <= b);
        CHECK(b <= c);
        CHECK(b <= d);
        CHECK_FALSE(b <= e);

        CHECK_FALSE(c <= a);
        CHECK_FALSE(c <= b);
        CHECK(c <= c);
        CHECK(c <= d);
        CHECK_FALSE(c <= e);

        CHECK_FALSE(d <= a);
        CHECK_FALSE(d <= b);
        CHECK_FALSE(d <= c);
        CHECK(d <= d);
        CHECK_FALSE(d <= e);

        CHECK(e <= a);
        CHECK(e <= b);
        CHECK(e <= c);
        CHECK(e <= d);
        CHECK(e <= e);
    }

    SECTION("operator >=")
    {
        CHECK(a >= a);
        CHECK_FALSE(a >= b);
        CHECK_FALSE(a >= c);
        CHECK_FALSE(a >= d);
        CHECK(a >= e);

        CHECK(b >= a);
        CHECK(b >= b);
        CHECK_FALSE(b >= c);
        CHECK_FALSE(b >= d);
        CHECK(b >= e);

        CHECK(c >= a);
        CHECK(c >= b);
        CHECK(c >= c);
        CHECK_FALSE(c >= d);
        CHECK(c >= e);

        CHECK(d >= a);
        CHECK(d >= b);
        CHECK(d >= c);
        CHECK(d >= d);
        CHECK(d >= e);

        CHECK(e >= a);
        CHECK_FALSE(e >= b);
        CHECK_FALSE(e >= c);
        CHECK_FALSE(e >= d);
        CHECK(e >= e);
    }
}

TEST_CASE("CodeEditor::Glyph")
{
    const dlxemu::CodeEditor::Glyph a{'a', dlxemu::CodeEditor::PaletteIndex::Default};
    CHECK(a.m_Char == 'a');
    CHECK(a.m_ColorIndex == dlxemu::CodeEditor::PaletteIndex::Default);

    const dlxemu::CodeEditor::Glyph b{'b', dlxemu::CodeEditor::PaletteIndex::OpCode};
    CHECK(b.m_Char == 'b');
    CHECK(b.m_ColorIndex == dlxemu::CodeEditor::PaletteIndex::OpCode);
}

TEST_CASE("CodeEditor")
{
    dlxemu::Emulator emulator;

    SECTION("Construct/Destruct")
    {
        dlxemu::CodeEditor editor{&emulator};
    }

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

    SECTION("Render")
    {
        // We can't really test here much expect that calling the function "works" and doesn't crash
        // TODO: Maybe this is an area where we could improve our test suite
        dlxemu::CodeEditor editor{&emulator};

        BeginImGui();
        editor.Render();
        editor.VerifyInternalState();
        EndImgui();

        // With size
        BeginImGui();
        editor.Render(ImVec2{1024.0f, 720.0f});
        editor.VerifyInternalState();
        EndImgui();

        // With border
        BeginImGui();
        editor.Render({}, true);
        editor.VerifyInternalState();
        EndImgui();

        // With size and border
        BeginImGui();
        editor.Render(ImVec2{1024.0f, 720.0f}, true);
        editor.VerifyInternalState();
        EndImgui();

        // With negative size
        BeginImGui();
        editor.Render(ImVec2{-100.0f, -100.0f});
        editor.VerifyInternalState();
        EndImgui();

        // With float max
        BeginImGui();
        static constexpr const float max_float = std::numeric_limits<float>::max();
        editor.Render(ImVec2{max_float, max_float});
        editor.VerifyInternalState();
        EndImgui();

        // With float min
        BeginImGui();
        static constexpr const float min_float = std::numeric_limits<float>::min();
        editor.Render(ImVec2{min_float, min_float});
        editor.VerifyInternalState();
        EndImgui();

        // With quiet NaN
        BeginImGui();
        static constexpr const float float_quiet_nan = std::numeric_limits<float>::quiet_NaN();
        editor.Render(ImVec2{float_quiet_nan, float_quiet_nan});
        editor.VerifyInternalState();
        EndImgui();

        // With signaling NaN
        BeginImGui();
        static constexpr const float float_signaling_nan =
                std::numeric_limits<float>::signaling_NaN();
        editor.Render(ImVec2{float_signaling_nan, float_signaling_nan});
        editor.VerifyInternalState();
        EndImgui();

        // With Infinity
        BeginImGui();
        static constexpr const float float_inf = std::numeric_limits<float>::infinity();
        editor.Render(ImVec2{float_inf, float_inf});
        editor.VerifyInternalState();
        EndImgui();
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

        // Empty text
        editor.SetText("");
        editor.VerifyInternalState();
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text.empty());
        CHECK(lines.size() == 1u);
        CHECK(lines.at(0u).empty());
        CHECK(editor.GetTotalLines() == 1u);
    }

    SECTION("ClearText")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Works with no text
        editor.ClearText();
        editor.VerifyInternalState();

        CHECK(editor.GetText().empty());
        CHECK_FALSE(editor.CanUndo());

        // Basic usage
        editor.SetText("Hello World");
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "Hello World");

        editor.SetCursorPosition({0u, 3u});
        editor.VerifyInternalState();

        editor.SelectAll();
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "Hello World");

        editor.ClearText();
        editor.VerifyInternalState();

        CHECK(editor.GetText().empty());
        CHECK_FALSE(editor.HasSelection());
        dlxemu::CodeEditor::Coordinates coords = editor.GetCursorPosition();
        CHECK(coords.m_Line == 0u);
        CHECK(coords.m_Column == 0u);

        // Undo restores correctly
        CHECK(editor.CanUndo());
        editor.Undo();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "Hello World");
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "Hello World");
        coords = editor.GetCursorPosition();
        CHECK(coords.m_Line == 0u);
        CHECK(coords.m_Column == 3u);

        // Redo works correctly
        CHECK(editor.CanRedo());
        editor.Redo();
        editor.VerifyInternalState();

        CHECK(editor.GetText().empty());
        CHECK_FALSE(editor.HasSelection());
        coords = editor.GetCursorPosition();
        CHECK(coords.m_Line == 0u);
        CHECK(coords.m_Column == 0u);

        // ClearText in ReadOnly mode does nothing
        editor.SetText("Hello World!");
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "Hello World!");

        editor.SetReadOnly(true);
        editor.VerifyInternalState();

        editor.SetCursorPosition({0u, 3u});
        editor.VerifyInternalState();

        editor.SelectAll();
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "Hello World!");

        editor.ClearText();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "Hello World!");
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "Hello World!");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 3u});
    }

    SECTION("SetTextLines")
    {
        dlxemu::CodeEditor       editor{&emulator};
        std::vector<std::string> new_lines;
        std::string              text;
        std::vector<std::string> lines;

        // single line
        new_lines.reserve(1u);
        new_lines.emplace_back("Hi");

        editor.SetTextLines(new_lines);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Hi");
        CHECK(lines.size() == 1u);
        CHECK(lines.at(0u) == "Hi");
        CHECK(editor.GetTotalLines() == 1u);

        // Two lines
        new_lines.clear();
        new_lines.reserve(2u);
        new_lines.emplace_back("Hello");
        new_lines.emplace_back("World");

        editor.SetTextLines(new_lines);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Hello\nWorld");
        CHECK(lines.size() == 2u);
        CHECK(lines.at(0u) == "Hello");
        CHECK(lines.at(1u) == "World");
        CHECK(editor.GetTotalLines() == 2u);

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

        // Test with embeded null characters
        new_lines.clear();
        new_lines.reserve(2u);
        new_lines.emplace_back("A single line with newline at the end\n");
        new_lines.emplace_back("Normal everyday line");

        editor.SetTextLines(new_lines);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "A single line with newline at the end\n\nNormal everyday line");
        CHECK(lines.size() == 3u);
        CHECK(lines.at(0u) == "A single line with newline at the end");
        CHECK(lines.at(1u).empty());
        CHECK(lines.at(2u) == "Normal everyday line");
        CHECK(editor.GetTotalLines() == 3u);

        // New line in the middle
        new_lines.clear();
        new_lines.reserve(1u);
        new_lines.emplace_back("Wow\nSetTextLines supports\nEmbedded new lines\nAwesome");

        editor.SetTextLines(new_lines);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Wow\nSetTextLines supports\nEmbedded new lines\nAwesome");
        CHECK(lines.size() == 4u);
        CHECK(lines.at(0u) == "Wow");
        CHECK(lines.at(1u) == "SetTextLines supports");
        CHECK(lines.at(2u) == "Embedded new lines");
        CHECK(lines.at(3u) == "Awesome");
        CHECK(editor.GetTotalLines() == 4u);
    }

    SECTION("GetSelectedText")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.SetText("WoW cool\ntext we have here");
        editor.VerifyInternalState();

        editor.SetSelection({0u, 0u}, {0u, 3u}); // Select "WoW"
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 3u});
        CHECK(editor.GetSelectedText() == "WoW");

        editor.SetSelection({0u, 4u}, {1u, 17u}); // Select "cool\ntext we have here"
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 4u});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1u, 17u});
        CHECK(editor.GetSelectedText() == "cool\ntext we have here");

        editor.SetSelection({1u, 1u}, {1u, 2u}); // Select "e"
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{1u, 1u});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1u, 2u});
        CHECK(editor.GetSelectedText() == "e");

        // No selection
        editor.ClearSelection();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 0u});
        CHECK(editor.GetSelectedText().empty());

        // Select all
        editor.SelectAll();
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1u, 17});
        CHECK(editor.GetText() == "WoW cool\ntext we have here");
        CHECK(editor.GetSelectedText() == "WoW cool\ntext we have here");
    }

    SECTION("GetCurrentLineText")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Default
        std::string line = editor.GetCurrentLineText();
        CHECK(line.empty());

        // With text
        editor.SetText("1.\n2.\n3.");
        editor.VerifyInternalState();

        line = editor.GetCurrentLineText();
        CHECK(line == "1.");

        // Same line different column
        editor.SetCursorPosition({0u, 1u});
        editor.VerifyInternalState();

        line = editor.GetCurrentLineText();
        CHECK(line == "1.");

        // Different line
        editor.SetCursorPosition({1u, 0u});
        editor.VerifyInternalState();

        line = editor.GetCurrentLineText();
        CHECK(line == "2.");

        // Last line
        editor.MoveBottom();
        editor.VerifyInternalState();

        line = editor.GetCurrentLineText();
        CHECK(line == "3.");
    }

    SECTION("GetTotalLines")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Default
        CHECK(editor.GetTotalLines() == 1u);

        editor.SetText("One line");
        editor.VerifyInternalState();

        CHECK(editor.GetTotalLines() == 1u);

        editor.SetText("Two\nLines");
        editor.VerifyInternalState();

        CHECK(editor.GetTotalLines() == 2u);

        editor.SetText("Three\nLines\nNow");
        editor.VerifyInternalState();

        CHECK(editor.GetTotalLines() == 3u);

        // Empty lines
        editor.SetText("\n\n\n\n\n\n\n\n\n");
        editor.VerifyInternalState();

        CHECK(editor.GetTotalLines() == 10u);
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

        editor.ToggleOverwrite();
        editor.VerifyInternalState();

        CHECK(editor.IsOverwrite());

        editor.ToggleOverwrite();
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

        editor.ToggleReadOnly();
        editor.VerifyInternalState();

        CHECK(editor.IsReadOnly());

        editor.ToggleReadOnly();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsReadOnly());
    }

    SECTION("IsTextChanged")
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        // Default
        CHECK_FALSE(editor.IsTextChanged());

        editor.EnterCharacter('A');
        editor.VerifyInternalState();

        CHECK(editor.IsTextChanged());

        // Cleared after rendering
        editor.Render();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsTextChanged());

        EndImgui();
    }

    SECTION("IsCursorPositionChanged")
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        // Default
        CHECK_FALSE(editor.IsTextChanged());

        // SetCursorPosition
        editor.SetText("Hello");
        editor.VerifyInternalState();

        editor.SetCursorPosition({0u, 3u});
        editor.VerifyInternalState();

        CHECK(editor.IsCursorPositionChanged());

        // Cleared after rendering
        editor.Render();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsCursorPositionChanged());

        EndImgui();
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

        editor.ToggleColorizerEnabled();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsColorizerEnabled());

        editor.ToggleColorizerEnabled();
        editor.VerifyInternalState();

        CHECK(editor.IsColorizerEnabled());
    }

    SECTION("Get/SetCursorPosition")
    {
        dlxemu::CodeEditor editor{&emulator};

        // Default
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

        editor.SetText("Some Text\nOn this line aswell");
        editor.VerifyInternalState();

        editor.SetCursorPosition({0u, 5u});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 5u});

        editor.SetCursorPosition({1u, 7u});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 7u});

        // Column is too big
        editor.SetCursorPosition({0u, 999u});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 9u});

        // Column negative
        dlxemu::CodeEditor::Coordinates coord;
        coord.m_Line   = 0u;
        coord.m_Column = -1;
        editor.SetCursorPosition(coord);
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

        // Line too big
        editor.SetCursorPosition({13u, 0u});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 19u});

        // Line negative
        coord.m_Line   = -1;
        coord.m_Column = 2u;
        editor.SetCursorPosition(coord);
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    }

    SECTION("ShowWhitespaces")
    {
        BeginImGui();

        dlxemu::CodeEditor editor{&emulator};

        CHECK_FALSE(editor.IsShowingWhitespaces());

        editor.SetShowWhitespaces(true);
        editor.VerifyInternalState();

        CHECK(editor.IsShowingWhitespaces());

        editor.Render();
        editor.VerifyInternalState();

        editor.SetText("This is a string with whitespaces.");
        editor.VerifyInternalState();
        editor.Render();
        editor.VerifyInternalState();

        CHECK(editor.IsShowingWhitespaces());

        editor.SetShowWhitespaces(false);
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsShowingWhitespaces());

        editor.Render();
        editor.VerifyInternalState();

        editor.ClearText();
        editor.VerifyInternalState();
        editor.Render();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsShowingWhitespaces());

        editor.ToggleShowWhitespaces();
        editor.VerifyInternalState();

        CHECK(editor.IsShowingWhitespaces());

        editor.ToggleShowWhitespaces();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.IsShowingWhitespaces());

        EndImgui();
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

        // Not skipping characters
        editor.SetReadOnly(false);
        editor.VerifyInternalState();
        editor.ClearText();
        editor.VerifyInternalState();
        editor.InsertText("\r\r\r");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "\r\r\r");
        CHECK(lines.size() == 1u);
        CHECK(lines.at(0u) == "\r\r\r");
        CHECK(editor.GetTotalLines() == 1u);

        // Insert text with tab character
        editor.ClearText();
        editor.VerifyInternalState();
        editor.InsertText("\x7F\t\a");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "\x7F\t\a");
        CHECK(lines.size() == 1u);
        CHECK(lines.at(0u) == "\x7F\t\a");
        CHECK(editor.GetTotalLines() == 1u);
        // Make sure were at the end
        dlxemu::CodeEditor::Coordinates coord = editor.GetCursorPosition();
        editor.MoveEnd();
        CHECK(editor.GetCursorPosition() == coord);
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

    SECTION("MoveDown")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.MoveDown(1);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.MoveDown(100);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.SetText("Line number 1 awesome\nLine 2\nLast Line is this and its pretty long wow");
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 0);
        CHECK(pos.m_Column == 0);

        editor.MoveDown();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 0);

        editor.MoveEnd();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 1);
        CHECK(pos.m_Column == 6);

        // Column is not changed when moving down and the last line has enough characters
        editor.MoveDown();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 6);

        // Moving down on the final line moves to the end of the line
        editor.MoveDown();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(pos.m_Line == 2);
        CHECK(pos.m_Column == 41);
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
