#include <phi/test/test_macros.hpp>

#include "SetupImGui.hpp"
#include <DLXEmu/CodeEditor.hpp>
#include <DLXEmu/Emulator.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>

PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5262)

#include <limits>

PHI_MSVC_SUPPRESS_WARNING_POP()

using namespace phi::literals;

PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")

static dlxemu::Emulator emulator;

// TODO: MoveX with 0 amount being noop
// TODO: Undo/Redo with 0

TEST_CASE("CodeEditor::Coordinates")
{
    const dlxemu::CodeEditor::Coordinates default_coord{};
    CHECK(default_coord.m_Line == 0u);
    CHECK(default_coord.m_Column == 0u);

    const dlxemu::CodeEditor::Coordinates coords{42u, 22u};
    CHECK(coords.m_Line == 42u);
    CHECK(coords.m_Column == 22u);

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

TEST_CASE("Construct/Destruct")
{
    dlxemu::CodeEditor editor{&emulator};
}

TEST_CASE("Set/GetPalette")
{
    BeginImGui();

    dlxemu::CodeEditor editor{&emulator};

    // Default
    dlxemu::CodeEditor::Palette dark_palette    = dlxemu::CodeEditor::GetDarkPalette();
    dlxemu::CodeEditor::Palette current_palette = editor.GetPalette();

    for (std::size_t i{0}; i < static_cast<std::size_t>(dlxemu::CodeEditor::PaletteIndex::Max); ++i)
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

    EndImGui();
}

TEST_CASE("ErrorMarkers")
{
    dlxemu::CodeEditor editor{&emulator};

    // Default
    dlxemu::CodeEditor::ErrorMarkers& markers = editor.GetErrorMarkers();
    CHECK(markers.empty());

    // Make sure the lines actually exist
    editor.SetText("\n\n\n\n\n");

    // SetErrorMarkers
    dlxemu::CodeEditor::ErrorMarkers test_markers;
    test_markers[1u] = "Test";
    test_markers[2u] = "Coolio";
    test_markers[3u] = "Hey";
    test_markers[4u] = "Awesome";

    editor.SetErrorMarkers(test_markers);
    editor.VerifyInternalState();
    markers = editor.GetErrorMarkers();

    CHECK(markers.size() == 4);
    CHECK(markers[1u] == "Test");
    CHECK(markers[2u] == "Coolio");
    CHECK(markers[3u] == "Hey");
    CHECK(markers[4u] == "Awesome");

    // ClearErrorMarkers
    editor.ClearErrorMarkers();
    editor.VerifyInternalState();

    markers = editor.GetErrorMarkers();

    CHECK(markers.empty());

    // AddErrorMarker
    editor.AddErrorMarker(1u, "Hey");
    editor.AddErrorMarker(1u, "Test");
    editor.AddErrorMarker(2u, "Coolio");

    markers = editor.GetErrorMarkers();

    CHECK(markers.size() == 2);
    CHECK(markers[1u] == "Hey\nTest");
    CHECK(markers[2u] == "Coolio");

    // const
    const dlxemu::CodeEditor const_editor{&emulator};

    const dlxemu::CodeEditor::ErrorMarkers& const_markers = const_editor.GetErrorMarkers();
    CHECK(const_markers.empty());
}

TEST_CASE("Breakpoints")
{
    dlxemu::CodeEditor editor{&emulator};

    // Default
    dlxemu::CodeEditor::Breakpoints& break_points = editor.GetBreakpoints();
    CHECK(break_points.empty());

    // Make sure the lines actually exist
    editor.SetText("\n\n\n\n\n");

    // SetBreakPoints
    dlxemu::CodeEditor::Breakpoints test;
    test.insert(1u);
    test.insert(3u);
    editor.SetBreakpoints(test);
    editor.VerifyInternalState();

    break_points = editor.GetBreakpoints();
    CHECK(break_points.size() == 2);
    CHECK(break_points.contains(1u));
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

TEST_CASE("Render")
{
    // We can't really test here much expect that calling the function "works" and doesn't crash
    // TODO: Maybe this is an area where we could improve our test suite
    dlxemu::CodeEditor editor{&emulator};

    // Default
    BeginImGui();
    editor.Render();
    editor.VerifyInternalState();
    EndImGui();

    // With size
    BeginImGui();
    editor.Render(ImVec2{1024.0f, 720.0f});
    editor.VerifyInternalState();
    EndImGui();

    // With border
    BeginImGui();
    editor.Render({}, true);
    editor.VerifyInternalState();
    EndImGui();

    // With size and border
    BeginImGui();
    editor.Render(ImVec2{1024.0f, 720.0f}, true);
    editor.VerifyInternalState();
    EndImGui();
}

TEST_CASE("Get/SetText")
{
    dlxemu::CodeEditor editor{&emulator};

    std::string              text  = editor.GetText();
    std::vector<std::string> lines = editor.GetTextLines();
    CHECK(text.empty());
    CHECK(lines.size() == 1);
    CHECK(lines.at(0).empty());
    CHECK(editor.GetTotalLines() == 1u);

    constexpr const char* t1{"A simple line"};
    editor.SetText(t1);
    editor.VerifyInternalState();
    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == t1);
    CHECK(lines.size() == 1);
    CHECK(lines.at(0) == t1);
    CHECK(editor.GetTotalLines() == 1u);

    constexpr const char* t2{"\n"};
    editor.SetText(t2);
    editor.VerifyInternalState();
    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == t2);
    CHECK(lines.size() == 2);
    CHECK(lines.at(0).empty());
    CHECK(lines.at(1).empty());
    CHECK(editor.GetTotalLines() == 2u);

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
    CHECK(editor.GetTotalLines() == 3u);

    constexpr const char* t4{"Multiple\nLines"};
    editor.SetText(t4);
    editor.VerifyInternalState();
    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == t4);
    CHECK(lines.size() == 2);
    CHECK(lines.at(0) == "Multiple");
    CHECK(lines.at(1) == "Lines");
    CHECK(editor.GetTotalLines() == 2u);

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
    CHECK(editor.GetTotalLines() == 4u);

    // Empty text
    editor.SetText("");
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text.empty());
    CHECK(lines.size() == 1u);
    CHECK(lines.at(0u).empty());
    CHECK(editor.GetTotalLines() == 1u);

    // SetText doesn't ignore tabs
    editor.SetText("\t");
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "\t");
    CHECK(lines.size() == 1u);
    CHECK(editor.GetTotalLines() == 1u);
    CHECK(lines.at(0u) == "\t");
}

TEST_CASE("ClearText")
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

TEST_CASE("SetTextLines")
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
    CHECK(editor.GetTotalLines() == 1u);

    // Test with embedded null characters
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

TEST_CASE("GetSelectedText")
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
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{1u, 17u});
    CHECK(editor.GetText() == "WoW cool\ntext we have here");
    CHECK(editor.GetSelectedText() == "WoW cool\ntext we have here");
}

TEST_CASE("GetCurrentLineText")
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

TEST_CASE("GetTotalLines")
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

TEST_CASE("Overwrite")
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

TEST_CASE("ReadOnly")
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

TEST_CASE("IsTextChanged")
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

    EndImGui();
}

TEST_CASE("IsCursorPositionChanged")
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

    EndImGui();
}

TEST_CASE("Colorizer")
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

TEST_CASE("Get/SetCursorPosition")
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

    // Line too big
    editor.SetCursorPosition({13u, 0u});
    editor.VerifyInternalState();

    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 19u});

    dlxemu::CodeEditor::Coordinates pos;

    editor.ClearText();
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100u, 100u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetText("123456789");
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0u, 5u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 5u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100u, 100u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 9u);

    editor.InsertText("\n123456789\n123456789");
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 2u);
    CHECK(pos.m_Column == 9u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1u, 7u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 7u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100u, 100u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 2u);
    CHECK(pos.m_Column == 9u);

    editor.SetText("");
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    // Correctly sanitize invalid positions inside a tab
    editor.SetText("\t");
    editor.VerifyInternalState();
    editor.SetCursorPosition({0u, 0u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetCursorPosition({0u, 1u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.SetCursorPosition({0u, 2u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.SetCursorPosition({0u, 3u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.SetCursorPosition({0u, 4u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    // Clamped
    editor.SetCursorPosition({0u, 5u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    // Test with 2 tabs
    editor.SetText("\t\t");
    editor.VerifyInternalState();

    editor.SetCursorPosition({0u, 0u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetCursorPosition({0u, 1u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.SetCursorPosition({0u, 2u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.SetCursorPosition({0u, 3u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.SetCursorPosition({0u, 4u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.SetCursorPosition({0u, 5u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 8u);

    editor.SetCursorPosition({0u, 6u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 8u);

    editor.SetCursorPosition({0u, 7u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 8u);

    editor.SetCursorPosition({0u, 8u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 8u);

    // Clamped
    editor.SetCursorPosition({0u, 9u});
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 8u);
}

TEST_CASE("ShowWhitespaces")
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

    EndImGui();
}

TEST_CASE("TabSize")
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

TEST_CASE("EnterCharacter")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.EnterCharacter('H');
    editor.VerifyInternalState();

    std::string              text  = editor.GetText();
    std::vector<std::string> lines = editor.GetTextLines();
    CHECK(text == "H");
    CHECK(lines.size() == 1);
    CHECK(lines.at(0) == "H");
    CHECK(editor.GetTotalLines() == 1u);

    editor.EnterCharacter('i');
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "Hi");
    CHECK(lines.size() == 1);
    CHECK(lines.at(0) == "Hi");
    CHECK(editor.GetTotalLines() == 1u);

    editor.MoveHome();
    editor.VerifyInternalState();
    editor.EnterCharacter('-');
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "-Hi");
    CHECK(lines.size() == 1);
    CHECK(lines.at(0) == "-Hi");
    CHECK(editor.GetTotalLines() == 1u);

    editor.SelectAll();
    editor.VerifyInternalState();
    editor.EnterCharacter('A');
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "A");
    CHECK(lines.size() == 1);
    CHECK(lines.at(0) == "A");
    CHECK(editor.GetTotalLines() == 1u);

    editor.EnterCharacter('\n');
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "A\n");
    CHECK(lines.size() == 2);
    CHECK(lines.at(0) == "A");
    CHECK(lines.at(1).empty());
    CHECK(editor.GetTotalLines() == 2u);

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
    CHECK(editor.GetTotalLines() == 2u);

    editor.EnterCharacter('C');
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "BC\n");
    CHECK(lines.size() == 2);
    CHECK(lines.at(0) == "BC");
    CHECK(lines.at(1).empty());
    CHECK(editor.GetTotalLines() == 2u);

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
    CHECK(editor.GetTotalLines() == 2u);

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
    CHECK(editor.GetTotalLines() == 2u);
    CHECK(lines.at(0u) == "BC");
    CHECK(lines.at(1u).empty());

    // Auto indent on new line
    editor.SetText("   ABC");
    editor.VerifyInternalState();
    editor.SetCursorPosition({0u, 999u});
    editor.VerifyInternalState();
    editor.EnterCharacter('\n');
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "   ABC\n   ");
    CHECK(lines.size() == 2u);
    CHECK(editor.GetTotalLines() == 2u);
    CHECK(lines.at(0u) == "   ABC");
    CHECK(lines.at(1u) == "   ");

    editor.EnterCharacter('\n');
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "   ABC\n   \n   ");
    CHECK(lines.size() == 3u);
    CHECK(editor.GetTotalLines() == 3u);
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
    CHECK(editor.GetTotalLines() == 1u);
    CHECK(lines.at(0u) == "\t\tTabedLine");

    // Shift tab removes tab
    editor.EnterCharacter('\t', true);
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "\tTabedLine");
    CHECK(lines.size() == 1u);
    CHECK(editor.GetTotalLines() == 1u);
    CHECK(lines.at(0u) == "\tTabedLine");

    editor.EnterCharacter('\t', true);
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "TabedLine");
    CHECK(lines.size() == 1u);
    CHECK(editor.GetTotalLines() == 1u);
    CHECK(lines.at(0u) == "TabedLine");

    // Shift tab does nothing when theres no tab
    editor.EnterCharacter('\t', true);
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "TabedLine");
    CHECK(lines.size() == 1u);
    CHECK(editor.GetTotalLines() == 1u);
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
    CHECK(editor.GetTotalLines() == 2u);
    CHECK(lines.at(0u) == "\tHi");
    CHECK(lines.at(1u) == "\t:)");

    // Remove multiline tab indent
    editor.EnterCharacter('\t', true);
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "Hi\n:)");
    CHECK(lines.size() == 2u);
    CHECK(editor.GetTotalLines() == 2u);
    CHECK(lines.at(0u) == "Hi");
    CHECK(lines.at(1u) == ":)");
}

TEST_CASE("Backspace")
{
    dlxemu::CodeEditor editor{&emulator};

    // With nothing to delete its a noop
    editor.Backspace();
    editor.VerifyInternalState();

    editor.InsertText(" \tA\nb\t \n");
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " \tA\nb\t \n");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{2u, 0u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " \tA\nb\t ");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 5u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " \tA\nb\t");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 4u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " \tA\nb");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 1u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " \tA\n");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 0u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " \tA");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 5u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " \t");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 4u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == " ");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 1u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText().empty());
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

    // Noop when read only
    editor.InsertText("New text");
    editor.VerifyInternalState();
    editor.SetReadOnly(true);
    editor.VerifyInternalState();
    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "New text");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 8u});

    // No auto indent when inserting before whitespaces
    editor.SetReadOnly(false);
    editor.VerifyInternalState();
    editor.SetText(" Text");
    editor.SetCursorPosition({0u, 0u});
    editor.VerifyInternalState();
    editor.EnterCharacter('\n');
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "\n Text");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{1u, 0u});

    // Correctly handled tabs
    editor.SetText("\t");
    editor.VerifyInternalState();
    editor.MoveEnd();
    editor.VerifyInternalState();
    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText().empty());
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

    editor.SetText("\tA");
    editor.VerifyInternalState();
    editor.MoveRight();
    editor.VerifyInternalState();
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 4u});

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "A");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

    editor.SetText("a\tA");
    editor.VerifyInternalState();
    editor.SetCursorPosition({0u, 4u});
    editor.VerifyInternalState();
    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "aA");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 1u});

    editor.SetText("ab\tA");
    editor.VerifyInternalState();
    editor.SetCursorPosition({0u, 4u});
    editor.VerifyInternalState();
    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "abA");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 2u});

    editor.SetText("abc\tA");
    editor.VerifyInternalState();
    editor.SetCursorPosition({0u, 4u});
    editor.VerifyInternalState();
    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "abcA");
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 3u});

    // Remove all
    editor.SetText("A\nB\nC\n");
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "A\nB\nC\n");
    CHECK_FALSE(editor.HasSelection());
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});

    editor.SelectAll();
    editor.VerifyInternalState();

    CHECK(editor.GetText() == "A\nB\nC\n");
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{3u, 0u});
    CHECK(editor.GetSelectedText() == "A\nB\nC\n");

    editor.Backspace();
    editor.VerifyInternalState();

    CHECK(editor.GetText().empty());
    CHECK(editor.GetCursorPosition() == dlxemu::CodeEditor::Coordinates{0u, 0u});
}

TEST_CASE("InsertText")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.InsertText("Hello World!");
    editor.VerifyInternalState();

    std::string              text  = editor.GetText();
    std::vector<std::string> lines = editor.GetTextLines();
    CHECK(text == "Hello World!");
    CHECK(lines.size() == 1);
    CHECK(lines.at(0) == "Hello World!");
    CHECK(editor.GetTotalLines() == 1u);

    editor.InsertText("\nTest string\n");
    editor.VerifyInternalState();

    text  = editor.GetText();
    lines = editor.GetTextLines();
    CHECK(text == "Hello World!\nTest string\n");
    CHECK(lines.size() == 3);
    CHECK(lines.at(0) == "Hello World!");
    CHECK(lines.at(1) == "Test string");
    CHECK(lines.at(2).empty());
    CHECK(editor.GetTotalLines() == 3u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(0u, 0u));
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
    CHECK(editor.GetTotalLines() == 3u);

    editor.SelectAll();
    editor.VerifyInternalState();
    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1u, 4u));
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
    CHECK(editor.GetTotalLines() == 3u);

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
    CHECK(editor.GetTotalLines() == 3u);

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
    CHECK(editor.GetTotalLines() == 3u);

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
    CHECK(editor.GetTotalLines() == 3u);

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

TEST_CASE("MoveUp")
{
    dlxemu::CodeEditor editor{&emulator};

    dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.MoveUp(1u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.MoveUp(100u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetText("Line 1\nLine 2\nLine 3\nLine 4\n");
    editor.VerifyInternalState();
    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(100u, 100u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 4u);
    CHECK(pos.m_Column == 0u);

    editor.MoveUp(1u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 3u);
    CHECK(pos.m_Column == 0u);

    editor.MoveUp(2u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 0u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(3u, 6u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 3u);
    CHECK(pos.m_Column == 6u);

    editor.MoveUp(1u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 2u);
    CHECK(pos.m_Column == 6u);

    editor.MoveUp(2u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 6u);

    editor.SetText("short line\nA very long line of text");
    editor.VerifyInternalState();
    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1u, 100u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 24u);

    editor.MoveUp();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 10u);

    editor.SetText("This is quite a long line of text wow\ncute\nAnother long line of text");
    editor.VerifyInternalState();
    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(2u, 25u));
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 2u);
    CHECK(pos.m_Column == 25u);

    editor.MoveUp();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 4u);

    editor.MoveUp();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 25u);

    editor.MoveUp();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetCursorPosition(dlxemu::CodeEditor::Coordinates(1u, 4u));
    editor.VerifyInternalState();
    editor.MoveUp(2u);
    editor.VerifyInternalState();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
}

TEST_CASE("MoveDown")
{
    dlxemu::CodeEditor editor{&emulator};

    dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.MoveDown(1u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.MoveDown(100u);
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetText("Line number 1 awesome\nLine 2\nLast Line is this and its pretty long wow");
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.MoveDown();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 0u);

    editor.MoveEnd();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 6u);

    // Column is not changed when moving down and the last line has enough characters
    editor.MoveDown();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 2u);
    CHECK(pos.m_Column == 6u);

    // Moving down on the final line moves to the end of the line
    editor.MoveDown();
    editor.VerifyInternalState();
    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 2u);
    CHECK(pos.m_Column == 41u);
}

TEST_CASE("MoveLeft")
{
    dlxemu::CodeEditor editor{&emulator};

    dlxemu::CodeEditor::Coordinates pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.MoveLeft();
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);

    editor.SetText("One line");
    editor.VerifyInternalState();
    editor.MoveEnd();
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 8u);

    editor.MoveLeft();
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 7u);

    editor.MoveLeft(4u);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 3u);

    editor.MoveLeft(25u, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectedText() == "One");
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 3u});

    // MoveLeft on the first line with selection but without shift clear the selection
    editor.MoveLeft(3u, false);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
    CHECK_FALSE(editor.HasSelection());

    // Word mode
    editor.MoveEnd();
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 8u);

    editor.MoveLeft(1u, false, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 4u);

    editor.MoveLeft(1u, true, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectedText() == "One ");
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 4u});

    // Move left on first line without shift clears selection
    editor.MoveLeft(1u, false, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
    CHECK_FALSE(editor.HasSelection());

    // Multiple lines
    editor.SetText("1\nTwo\n\n");
    editor.VerifyInternalState();
    editor.MoveBottom();
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 3u);
    CHECK(pos.m_Column == 0u);

    editor.MoveLeft();
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 2u);
    CHECK(pos.m_Column == 0u);

    editor.MoveLeft(1u, false, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 3u);

    editor.MoveLeft(1u, true, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 1u);
    CHECK(pos.m_Column == 0u);
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectedText() == "Two");

    editor.MoveLeft(1u, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 1u);
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectedText() == "\nTwo");

    editor.MoveLeft(1u, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectedText() == "1\nTwo");

    // Should basically be a nop
    editor.MoveLeft(1u, true);
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectedText() == "1\nTwo");

    // Absurdly large number
    editor.MoveLeft(std::numeric_limits<phi::u32>::max());
    editor.VerifyInternalState();

    pos = editor.GetCursorPosition();
    CHECK(pos.m_Line == 0u);
    CHECK(pos.m_Column == 0u);
    CHECK_FALSE(editor.HasSelection());
}

TEST_CASE("SetSelectionStart")
{
    dlxemu::CodeEditor editor{&emulator};

    editor.SetText("My text");
    editor.VerifyInternalState();
    editor.SetSelectionStart({0u, 3u});
    editor.VerifyInternalState();

    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 0u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 3u});

    editor.SetSelectionStart({0u, 1u});
    editor.VerifyInternalState();

    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 1u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 3u});

    // Too big line
    editor.SetSelectionStart({999u, 0u});
    editor.VerifyInternalState();

    CHECK(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 3u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 7u});

    // Too big column
    editor.SetSelectionStart({0u, 999u});
    editor.VerifyInternalState();

    CHECK_FALSE(editor.HasSelection());
    CHECK(editor.GetSelectionStart() == dlxemu::CodeEditor::Coordinates{0u, 7u});
    CHECK(editor.GetSelectionEnd() == dlxemu::CodeEditor::Coordinates{0u, 7u});
}

TEST_CASE("CodeEditor bad calls")
{
    SECTION("SetSelectionStart line very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = std::numeric_limits<phi::u32>::max();
        coords.m_Column = 0u;

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart column very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = 0u;
        coords.m_Column = std::numeric_limits<phi::u32>::max();

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetSelectionStart both very high")
    {
        dlxemu::CodeEditor editor{&emulator};

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = std::numeric_limits<phi::u32>::max();
        coords.m_Column = std::numeric_limits<phi::u32>::max();

        editor.SetSelectionEnd(coords);
        editor.VerifyInternalState();
    }

    SECTION("SetText and delete with control character")
    {
        dlxemu::CodeEditor editor{&emulator};

        editor.SetText("\x1E");
        editor.VerifyInternalState();

        dlxemu::CodeEditor::Coordinates coords;
        coords.m_Line   = 0u;
        coords.m_Column = 10u;

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
        coords.m_Line   = 0u;
        coords.m_Column = 7935u;

        editor.SetCursorPosition(coords);
        editor.VerifyInternalState();
        editor.Delete();
        editor.VerifyInternalState();
    }
}
