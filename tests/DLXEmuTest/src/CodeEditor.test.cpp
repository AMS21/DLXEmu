#include <catch2/catch_test_macros.hpp>

#include "SetupImGui.hpp"
#include <DLXEmu/CodeEditor.hpp>
#include <DLXEmu/Emulator.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>
#include <limits>

using namespace phi::literals;

// TODO: MoveX with 0 amount being noop
// TODO: Undo/Redo with 0

TEST_CASE("CodeEditor::Coordinates")
{
    const dlxemu::CodeEditor::Coordinates default_coord{};
    CHECK(bool(default_coord.m_Line == 0));
    CHECK(bool(default_coord.m_Column == 0));

    const dlxemu::CodeEditor::Coordinates coords{42, 22};
    CHECK(bool(coords.m_Line == 42));
    CHECK(bool(coords.m_Column == 22));

    const dlxemu::CodeEditor::Coordinates invalid{dlxemu::CodeEditor::Coordinates::Invalid()};
    CHECK(bool(invalid.m_Line == -1));
    CHECK(bool(invalid.m_Column == -1));

    const dlxemu::CodeEditor::Coordinates a{1, 0};
    const dlxemu::CodeEditor::Coordinates b{1, 1};
    const dlxemu::CodeEditor::Coordinates c{2, 0};
    const dlxemu::CodeEditor::Coordinates d{2, 2};
    const dlxemu::CodeEditor::Coordinates e{1, 0};

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
        test_markers[0u]  = "Test";
        test_markers[3u]  = "Coolio";
        test_markers[12u] = "Hey";
        test_markers[21u] = "Awesome";

        editor.SetErrorMarkers(test_markers);
        editor.VerifyInternalState();
        markers = editor.GetErrorMarkers();

        CHECK(markers.size() == 4);
        CHECK(markers[0u] == "Test");
        CHECK(markers[3u] == "Coolio");
        CHECK(markers[12u] == "Hey");
        CHECK(markers[21u] == "Awesome");

        editor.ClearErrorMarkers();
        editor.VerifyInternalState();

        // AddErrorMarker
        editor.AddErrorMarker(0u, "Hey");
        editor.AddErrorMarker(0u, "Test");
        editor.AddErrorMarker(2u, "Coolio");

        markers = editor.GetErrorMarkers();

        CHECK(markers.size() == 2);
        CHECK(markers[0u] == "Hey\nTest");
        CHECK(markers[2u] == "Coolio");

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
        test.insert(0u);
        test.insert(3u);
        editor.SetBreakpoints(test);
        editor.VerifyInternalState();

        break_points = editor.GetBreakpoints();
        CHECK(break_points.size() == 2);
        CHECK(break_points.contains(0u));
        CHECK(break_points.contains(3u));

        // ClearBreakpoints
        editor.ClearBreakPoints();
        editor.VerifyInternalState();

        CHECK(break_points.empty());

        // AddBreakpoint
        CHECK(editor.AddBreakpoint(3u));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3u));

        CHECK_FALSE(editor.AddBreakpoint(3u));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3u));

        CHECK(editor.AddBreakpoint(5u));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 2);
        CHECK(break_points.contains(3u));
        CHECK(break_points.contains(5u));

        // RemoveBreakpoint
        CHECK(editor.RemoveBreakpoint(5u));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3u));

        CHECK_FALSE(editor.RemoveBreakpoint(5u));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3u));

        // ToggleBreakpoint
        CHECK(editor.ToggleBreakpoint(5u));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 2);
        CHECK(break_points.contains(3u));
        CHECK(break_points.contains(5u));

        CHECK_FALSE(editor.ToggleBreakpoint(5u));
        editor.VerifyInternalState();
        break_points = editor.GetBreakpoints();

        REQUIRE(break_points.size() == 1);
        CHECK(break_points.contains(3u));

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
        CHECK(bool(editor.GetTotalLines() == 1u));

        constexpr const char* t1{"A simple line"};
        editor.SetText(t1);
        editor.VerifyInternalState();
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t1);
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == t1);
        CHECK(bool(editor.GetTotalLines() == 1u));

        constexpr const char* t2{"\n"};
        editor.SetText(t2);
        editor.VerifyInternalState();
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t2);
        CHECK(lines.size() == 2);
        CHECK(lines.at(0).empty());
        CHECK(lines.at(1).empty());
        CHECK(bool(editor.GetTotalLines() == 2u));

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
        CHECK(bool(editor.GetTotalLines() == 3u));

        constexpr const char* t4{"Multiple\nLines"};
        editor.SetText(t4);
        editor.VerifyInternalState();
        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == t4);
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "Multiple");
        CHECK(lines.at(1) == "Lines");
        CHECK(bool(editor.GetTotalLines() == 2u));

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
        CHECK(bool(editor.GetTotalLines() == 4u));

        // Empty text
        editor.SetText("");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text.empty());
        CHECK(lines.size() == 1u);
        CHECK(lines.at(0u).empty());
        CHECK(bool(editor.GetTotalLines() == 1u));

        // SetText doesn't ignore tabs
        editor.SetText("\t");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "\t");
        CHECK(lines.size() == 1u);
        CHECK(bool(editor.GetTotalLines() == 1u));
        CHECK(lines.at(0u) == "\t");
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

        editor.SetCursorPosition({0, 3});
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
        CHECK(bool(coords.m_Line == 0));
        CHECK(bool(coords.m_Column == 0));

        // Undo restores correctly
        CHECK(editor.CanUndo());
        editor.Undo();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "Hello World");
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "Hello World");
        coords = editor.GetCursorPosition();
        CHECK(bool(coords.m_Line == 0));
        CHECK(bool(coords.m_Column == 3));

        // Redo works correctly
        CHECK(editor.CanRedo());
        editor.Redo();
        editor.VerifyInternalState();

        CHECK(editor.GetText().empty());
        CHECK_FALSE(editor.HasSelection());
        coords = editor.GetCursorPosition();
        CHECK(bool(coords.m_Line == 0));
        CHECK(bool(coords.m_Column == 0));

        // ClearText in ReadOnly mode does nothing
        editor.SetText("Hello World!");
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "Hello World!");

        editor.SetReadOnly(true);
        editor.VerifyInternalState();

        editor.SetCursorPosition({0, 3});
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
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 3});
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
        CHECK(bool(editor.GetTotalLines() == 1u));

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
        CHECK(bool(editor.GetTotalLines() == 2u));

        new_lines.clear();
        new_lines.reserve(0);

        editor.SetTextLines(new_lines);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text.empty());
        CHECK(lines.size() == 1);
        CHECK(lines.at(0).empty());
        CHECK(bool(editor.GetTotalLines() == 1u));

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
        CHECK(bool(editor.GetTotalLines() == 3u));

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
        CHECK(bool(editor.GetTotalLines() == 4u));
    }

    SECTION("GetSelectedText")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.SetText("WoW cool\ntext we have here");
        editor.VerifyInternalState();

        editor.SetSelection({0, 0}, {0, 3}); // Select "WoW"
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 3});
        CHECK(editor.GetSelectedText() == "WoW");

        editor.SetSelection({0, 4}, {1, 17}); // Select "cool\ntext we have here"
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 4});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1, 17});
        CHECK(editor.GetSelectedText() == "cool\ntext we have here");

        editor.SetSelection({1, 1}, {1, 2}); // Select "e"
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{1, 1});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1, 2});
        CHECK(editor.GetSelectedText() == "e");

        // No selection
        editor.ClearSelection();
        editor.VerifyInternalState();

        CHECK_FALSE(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectedText().empty());

        // Select all
        editor.SelectAll();
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1, 17});
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
        editor.SetCursorPosition({0, 1});
        editor.VerifyInternalState();

        line = editor.GetCurrentLineText();
        CHECK(line == "1.");

        // Different line
        editor.SetCursorPosition({1, 0});
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
        CHECK(bool(editor.GetTotalLines() == 1u));

        editor.SetText("One line");
        editor.VerifyInternalState();

        CHECK(bool(editor.GetTotalLines() == 1u));

        editor.SetText("Two\nLines");
        editor.VerifyInternalState();

        CHECK(bool(editor.GetTotalLines() == 2u));

        editor.SetText("Three\nLines\nNow");
        editor.VerifyInternalState();

        CHECK(bool(editor.GetTotalLines() == 3u));

        // Empty lines
        editor.SetText("\n\n\n\n\n\n\n\n\n");
        editor.VerifyInternalState();

        CHECK(bool(editor.GetTotalLines() == 10u));
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

        editor.SetCursorPosition({0, 3});
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
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});

        editor.SetText("Some Text\nOn this line aswell");
        editor.VerifyInternalState();

        editor.SetCursorPosition({0, 5});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 5});

        editor.SetCursorPosition({1, 7});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1, 7});

        // Column is too big
        editor.SetCursorPosition({0, 999});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 9});

        // Column negative
        dlxemu::CodeEditor::Coordinates coord;
        coord.m_Line   = 0;
        coord.m_Column = -1;
        editor.SetCursorPosition(coord);
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});

        // Line too big
        editor.SetCursorPosition({13, 0});
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1, 19});

        // Line negative
        coord.m_Line   = -1;
        coord.m_Column = 2;
        editor.SetCursorPosition(coord);
        editor.VerifyInternalState();

        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});

        dlxemu::CodeEditor::Coordinates pos;

        editor.ClearText();
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetText("123456789");
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0, 5));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 5));

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 9));

        editor.InsertText("\n123456789\n123456789");
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 2));
        CHECK(bool(pos.m_Column == 9));

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 7));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 7));

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 2));
        CHECK(bool(pos.m_Column == 9));

        editor.SetText("");
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        // Correctly sanitize invalid positions inside a tab
        editor.SetText("\t");
        editor.VerifyInternalState();
        editor.SetCursorPosition({0, 0});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetCursorPosition({0, 1});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.SetCursorPosition({0, 2});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.SetCursorPosition({0, 3});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.SetCursorPosition({0, 4});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        // Clamped
        editor.SetCursorPosition({0, 5});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        // Test with 2 tabs
        editor.SetText("\t\t");
        editor.VerifyInternalState();

        editor.SetCursorPosition({0, 0});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetCursorPosition({0, 1});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.SetCursorPosition({0, 2});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.SetCursorPosition({0, 3});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.SetCursorPosition({0, 4});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.SetCursorPosition({0, 5});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 8));

        editor.SetCursorPosition({0, 6});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 8));

        editor.SetCursorPosition({0, 7});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 8));

        editor.SetCursorPosition({0, 8});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 8));

        // Clamped
        editor.SetCursorPosition({0, 9});
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 8));
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

    SECTION("TabSize")
    {
        dlxemu::CodeEditor editor{&emulator};

        CHECK(editor.GetTabSize().unsafe() == 4);

        editor.SetTabSize(1_u8);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize().unsafe() == 1);

        editor.SetTabSize(5_u8);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize().unsafe() == 5);

        // Clamped properly
        editor.SetTabSize(0_u8);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize().unsafe() == 1);

        editor.SetTabSize(100_u8);
        editor.VerifyInternalState();
        CHECK(editor.GetTabSize().unsafe() == 32);
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
        CHECK(bool(editor.GetTotalLines() == 1u));

        editor.EnterCharacter('i');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Hi");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "Hi");
        CHECK(bool(editor.GetTotalLines() == 1u));

        editor.MoveHome();
        editor.VerifyInternalState();
        editor.EnterCharacter('-');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "-Hi");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "-Hi");
        CHECK(bool(editor.GetTotalLines() == 1u));

        editor.SelectAll();
        editor.VerifyInternalState();
        editor.EnterCharacter('A');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "A");
        CHECK(lines.size() == 1);
        CHECK(lines.at(0) == "A");
        CHECK(bool(editor.GetTotalLines() == 1u));

        editor.EnterCharacter('\n');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "A\n");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "A");
        CHECK(lines.at(1).empty());
        CHECK(bool(editor.GetTotalLines() == 2u));

        editor.SetOverwrite(true);
        editor.VerifyInternalState();
        editor.MoveUp(31000u);
        editor.VerifyInternalState();
        editor.EnterCharacter('B');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "B\n");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "B");
        CHECK(lines.at(1).empty());
        CHECK(bool(editor.GetTotalLines() == 2u));

        editor.EnterCharacter('C');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "BC\n");
        CHECK(lines.size() == 2);
        CHECK(lines.at(0) == "BC");
        CHECK(lines.at(1).empty());
        CHECK(bool(editor.GetTotalLines() == 2u));

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
        CHECK(bool(editor.GetTotalLines() == 2u));

        // Entering '\0' does nothing
        editor.SetReadOnly(false);
        editor.VerifyInternalState();
        editor.EnterCharacter('\0');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK_FALSE(editor.IsReadOnly());
        CHECK(text == "BC\n");
        CHECK(lines.size() == 2u);
        CHECK(bool(editor.GetTotalLines() == 2u));
        CHECK(lines.at(0u) == "BC");
        CHECK(lines.at(1u).empty());

        // Auto indent on new line
        editor.SetText("   ABC");
        editor.VerifyInternalState();
        editor.SetCursorPosition({0, 999});
        editor.VerifyInternalState();
        editor.EnterCharacter('\n');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "   ABC\n   ");
        CHECK(lines.size() == 2u);
        CHECK(bool(editor.GetTotalLines() == 2u));
        CHECK(lines.at(0u) == "   ABC");
        CHECK(lines.at(1u) == "   ");

        editor.EnterCharacter('\n');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "   ABC\n   \n   ");
        CHECK(lines.size() == 3u);
        CHECK(bool(editor.GetTotalLines() == 3u));
        CHECK(lines.at(0u) == "   ABC");
        CHECK(lines.at(1u) == "   ");
        CHECK(lines.at(2u) == "   ");

        // Tab Indent on selected text
        editor.SetText("\tTabedLine");
        editor.VerifyInternalState();
        editor.SelectAll();
        editor.VerifyInternalState();
        editor.EnterCharacter('\t');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "\t\tTabedLine");
        CHECK(lines.size() == 1u);
        CHECK(bool(editor.GetTotalLines() == 1u));
        CHECK(lines.at(0u) == "\t\tTabedLine");

        // Shift tab removes tab
        editor.EnterCharacter('\t', true);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "\tTabedLine");
        CHECK(lines.size() == 1u);
        CHECK(bool(editor.GetTotalLines() == 1u));
        CHECK(lines.at(0u) == "\tTabedLine");

        editor.EnterCharacter('\t', true);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "TabedLine");
        CHECK(lines.size() == 1u);
        CHECK(bool(editor.GetTotalLines() == 1u));
        CHECK(lines.at(0u) == "TabedLine");

        // Shift tab does nothing when theres no tab
        editor.EnterCharacter('\t', true);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "TabedLine");
        CHECK(lines.size() == 1u);
        CHECK(bool(editor.GetTotalLines() == 1u));
        CHECK(lines.at(0u) == "TabedLine");

        // Multiline tab indent
        editor.SetText("Hi\n:)");
        editor.VerifyInternalState();
        editor.SelectAll();
        editor.VerifyInternalState();
        editor.EnterCharacter('\t');
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "\tHi\n\t:)");
        CHECK(lines.size() == 2u);
        CHECK(bool(editor.GetTotalLines() == 2u));
        CHECK(lines.at(0u) == "\tHi");
        CHECK(lines.at(1u) == "\t:)");

        // Remove multiline tab indent
        editor.EnterCharacter('\t', true);
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Hi\n:)");
        CHECK(lines.size() == 2u);
        CHECK(bool(editor.GetTotalLines() == 2u));
        CHECK(lines.at(0u) == "Hi");
        CHECK(lines.at(1u) == ":)");
    }

    SECTION("Backspace")
    {
        dlxemu::CodeEditor editor{&emulator};

        // With nothing to delete its a noop
        editor.Backspace();
        editor.VerifyInternalState();

        editor.InsertText(" \tA\nb\t \n");
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " \tA\nb\t \n");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{2, 0});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " \tA\nb\t ");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1, 5});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " \tA\nb\t");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1, 4});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " \tA\nb");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1, 1});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " \tA\n");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1, 0});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " \tA");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 5});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " \t");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 4});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == " ");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 1});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText().empty());
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});

        // Noop when read only
        editor.InsertText("New text");
        editor.VerifyInternalState();
        editor.SetReadOnly(true);
        editor.VerifyInternalState();
        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "New text");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 8});

        // No auto indent when inserting before whitespaces
        editor.SetReadOnly(false);
        editor.VerifyInternalState();
        editor.SetText(" Text");
        editor.SetCursorPosition({0, 0});
        editor.VerifyInternalState();
        editor.EnterCharacter('\n');
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "\n Text");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1, 0});

        // Correctly handled tabs
        editor.SetText("\t");
        editor.VerifyInternalState();
        editor.MoveEnd();
        editor.VerifyInternalState();
        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText().empty());
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});

        editor.SetText("\tA");
        editor.VerifyInternalState();
        editor.MoveRight();
        editor.VerifyInternalState();
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 4});

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "A");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});

        editor.SetText("a\tA");
        editor.VerifyInternalState();
        editor.SetCursorPosition({0, 4});
        editor.VerifyInternalState();
        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "aA");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 1});

        editor.SetText("ab\tA");
        editor.VerifyInternalState();
        editor.SetCursorPosition({0, 4});
        editor.VerifyInternalState();
        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "abA");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 2});

        editor.SetText("abc\tA");
        editor.VerifyInternalState();
        editor.SetCursorPosition({0, 4});
        editor.VerifyInternalState();
        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "abcA");
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 3});

        // Remove all
        editor.SetText("A\nB\nC\n");
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "A\nB\nC\n");
        CHECK_FALSE(editor.HasSelection());
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});

        editor.SelectAll();
        editor.VerifyInternalState();

        CHECK(editor.GetText() == "A\nB\nC\n");
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{3, 0});
        CHECK(editor.GetSelectedText() == "A\nB\nC\n");

        editor.Backspace();
        editor.VerifyInternalState();

        CHECK(editor.GetText().empty());
        CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0, 0});
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
        CHECK(bool(editor.GetTotalLines() == 1u));

        editor.InsertText("\nTest string\n");
        editor.VerifyInternalState();

        text  = editor.GetText();
        lines = editor.GetTextLines();
        CHECK(text == "Hello World!\nTest string\n");
        CHECK(lines.size() == 3);
        CHECK(lines.at(0) == "Hello World!");
        CHECK(lines.at(1) == "Test string");
        CHECK(lines.at(2).empty());
        CHECK(bool(editor.GetTotalLines() == 3u));

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
        CHECK(bool(editor.GetTotalLines() == 3u));

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
        CHECK(bool(editor.GetTotalLines() == 3u));

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
        CHECK(bool(editor.GetTotalLines() == 3u));

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
        CHECK(bool(editor.GetTotalLines() == 3u));

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
        CHECK(bool(editor.GetTotalLines() == 3u));

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
        CHECK(bool(editor.GetTotalLines() == 1u));

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
        CHECK(bool(editor.GetTotalLines() == 1u));
        // Make sure were at the end
        dlxemu::CodeEditor::Coordinates coord = editor.GetCursorPosition();
        editor.MoveEnd();
        CHECK(editor.GetCursorPosition() == coord);
    }

    SECTION("MoveUp")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveUp(1u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveUp(100u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetText("Line 1\nLine 2\nLine 3\nLine 4\n");
        editor.VerifyInternalState();
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 4));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveUp(1u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 3));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveUp(2u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 0));

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(3, 6));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 3));
        CHECK(bool(pos.m_Column == 6));

        editor.MoveUp(1u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 2));
        CHECK(bool(pos.m_Column == 6));

        editor.MoveUp(2u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 6));

        editor.SetText("short line\nA very long line of text");
        editor.VerifyInternalState();
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 100));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 24));

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 10));

        editor.SetText("This is quite a long line of text wow\ncute\nAnother long line of text");
        editor.VerifyInternalState();
        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(2, 25));
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 2));
        CHECK(bool(pos.m_Column == 25));

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 4));

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 25));

        editor.MoveUp();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1, 4));
        editor.VerifyInternalState();
        editor.MoveUp(2u);
        editor.VerifyInternalState();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
    }

    SECTION("MoveDown")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveDown(1u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveDown(100u);
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetText("Line number 1 awesome\nLine 2\nLast Line is this and its pretty long wow");
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveDown();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveEnd();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 6));

        // Column is not changed when moving down and the last line has enough characters
        editor.MoveDown();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 2));
        CHECK(bool(pos.m_Column == 6));

        // Moving down on the final line moves to the end of the line
        editor.MoveDown();
        editor.VerifyInternalState();
        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 2));
        CHECK(bool(pos.m_Column == 41));
    }

    SECTION("MoveLeft")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveLeft();
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));

        editor.SetText("One line");
        editor.VerifyInternalState();
        editor.MoveEnd();
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 8));

        editor.MoveLeft();
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 7));

        editor.MoveLeft(4u);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 3));

        editor.MoveLeft(25u, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "One");
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 3});

        // MoveLeft on the first line with selection but without shift clear the selection
        editor.MoveLeft(3u, false);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
        CHECK_FALSE(editor.HasSelection());

        // Word mode
        editor.MoveEnd();
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 8));

        editor.MoveLeft(1u, false, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 4));

        editor.MoveLeft(1u, true, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "One ");
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 4});

        // Move left on first line without shift clears selection
        editor.MoveLeft(1u, false, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
        CHECK_FALSE(editor.HasSelection());

        // Multiple lines
        editor.SetText("1\nTwo\n\n");
        editor.VerifyInternalState();
        editor.MoveBottom();
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 3));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveLeft();
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 2));
        CHECK(bool(pos.m_Column == 0));

        editor.MoveLeft(1u, false, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 3));

        editor.MoveLeft(1u, true, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 1));
        CHECK(bool(pos.m_Column == 0));
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "Two");

        editor.MoveLeft(1u, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 1));
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "\nTwo");

        editor.MoveLeft(1u, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "1\nTwo");

        // Should basically be a nop
        editor.MoveLeft(1u, true);
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectedText() == "1\nTwo");

        // Absurdly large number
        editor.MoveLeft(std::numeric_limits<phi::u32>::max());
        editor.VerifyInternalState();

        pos = editor.GetCursorPosition();
        CHECK(bool(pos.m_Line == 0));
        CHECK(bool(pos.m_Column == 0));
        CHECK_FALSE(editor.HasSelection());
    }

    SECTION("SetSelectionStart")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.SetText("My text");
        editor.VerifyInternalState();
        editor.SetSelectionStart({0, 3});
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 0});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 3});

        editor.SetSelectionStart({0, 1});
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 1});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 3});

        // Too big line
        editor.SetSelectionStart({999, 0});
        editor.VerifyInternalState();

        CHECK(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 3});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 7});

        // Too big column
        editor.SetSelectionStart({0, 999});
        editor.VerifyInternalState();

        CHECK_FALSE(editor.HasSelection());
        CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0, 7});
        CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0, 7});
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
        coords.m_Line   = std::numeric_limits<phi::i32>::max();
        coords.m_Column = 0;

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart column very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = 0;
        coords.m_Column = std::numeric_limits<phi::i32>::max();

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart both very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = std::numeric_limits<phi::i32>::max();
        coords.m_Column = std::numeric_limits<phi::i32>::max();

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
