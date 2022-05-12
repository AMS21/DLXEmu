// This file is heavily inspired by BalazsJako ImGuiColorTextEdit library https://github.com/BalazsJako/ImGuiColorTextEdit
// licensed under the MIT license https://github.com/BalazsJako/ImGuiColorTextEdit/blob/master/LICENSE
// Original file at https://github.com/BalazsJako/ImGuiColorTextEdit/blob/master/TextEditor.cpp
/* MIT License

Copyright (c) 2017 BalazsJako

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "DLXEmu/CodeEditor.hpp"

#include "DLXEmu/Emulator.hpp"
#include <DLX/Parser.hpp>
#include <DLX/ParserUtils.hpp>
#include <DLX/Token.hpp>
#include <magic_enum.hpp>
#include <phi/algorithm/clamp.hpp>
#include <phi/algorithm/max.hpp>
#include <phi/algorithm/string_length.hpp>
#include <phi/algorithm/swap.hpp>
#include <phi/container/array.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/conversion.hpp>
#include <phi/core/size_t.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <phi/math/is_nan.hpp>
#include <phi/text/is_alpha_numeric.hpp>
#include <phi/text/is_blank.hpp>
#include <phi/text/is_control.hpp>
#include <phi/text/is_space.hpp>
#include <spdlog/fmt/bundled/core.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h> // for imGui::GetCurrentWindow()
#include <imgui_internal.h>

//#define DLXEMU_VERIFY_UNDO_REDO
//#define DLXEMU_VERIFY_COLUMN

// Free Helper functions

[[nodiscard]] static constexpr bool IsUTFSequence(const char character) noexcept
{
    return (character & 0xC0) == 0x80;
}

[[nodiscard]] static constexpr bool IsUTFSequence(const unsigned char character) noexcept
{
    return IsUTFSequence(static_cast<char>(character));
}

// https://en.wikipedia.org/wiki/UTF-8
// We assume that the char is a standalone character (<128) or a leading byte of an UTF-8 code sequence (non-10xxxxxx code)
[[nodiscard]] static constexpr phi::i32 UTF8CharLength(const char characater) noexcept
{
    if ((characater & 0xFE) == 0xFC)
    {
        return 6;
    }

    if ((characater & 0xFC) == 0xF8)
    {
        return 5;
    }

    if ((characater & 0xF8) == 0xF0)
    {
        return 4;
    }

    if ((characater & 0xF0) == 0xE0)
    {
        return 3;
    }

    if ((characater & 0xE0) == 0xC0)
    {
        return 2;
    }

    return 1;
}

[[nodiscard]] static constexpr phi::i32 UTF8CharLength(const unsigned char characater) noexcept
{
    return UTF8CharLength(static_cast<char>(characater));
}

using namespace phi::literals;

// "Borrowed" from ImGui source
static inline phi::u8_fast ImTextCharToUtf8(phi::array<char, 5u>& buffer,
                                            const phi::u32        character) noexcept
{
    if (character < 0x80u)
    {
        buffer[0u] = static_cast<char>(character.unsafe());
        buffer[1u] = '\0';
        return 1_u8;
    }
    if (character < 0x800u)
    {
        buffer[0u] = static_cast<char>(0xc0 + (character.unsafe() >> 6));
        buffer[1u] = static_cast<char>(0x80 + (character.unsafe() & 0x3f));
        buffer[2u] = '\0';

        return 2_u8;
    }
    if (character >= 0xdc00u && character < 0xe000u)
    {
        buffer[0u] = '\0';
        return 0_u8;
    }

    if (character >= 0xd800u && character < 0xdc00u)
    {
        buffer[0u] = static_cast<char>(0xf0 + (character.unsafe() >> 18));
        buffer[1u] = static_cast<char>(0x80 + ((character.unsafe() >> 12) & 0x3f));
        buffer[2u] = static_cast<char>(0x80 + ((character.unsafe() >> 6) & 0x3f));
        buffer[3u] = static_cast<char>(0x80 + (character.unsafe() & 0x3f));
        buffer[4u] = '\0';

        return 4_u8;
    }
    //else if (c < 0x10000)
    {
        buffer[0u] = static_cast<char>(0xe0 + (character.unsafe() >> 12));
        buffer[1u] = static_cast<char>(0x80 + ((character.unsafe() >> 6) & 0x3f));
        buffer[2u] = static_cast<char>(0x80 + (character.unsafe() & 0x3f));
        buffer[3u] = '\0';

        return 3_u8;
    }
}

[[nodiscard]] static constexpr phi::boolean IsValidUTF8Sequence(const phi::u32 character) noexcept
{
    return !(character >= 0xdc00u && character < 0xe000u);
}

namespace dlxemu
{
    // Coordinates
    CodeEditor::Coordinates::Coordinates() noexcept
        : m_Line(0)
        , m_Column(0)
    {}

    CodeEditor::Coordinates::Coordinates(phi::i32 line, phi::i32 column) noexcept
        : m_Line(line)
        , m_Column(column)
    {
        PHI_DBG_ASSERT(line >= 0);
        PHI_DBG_ASSERT(column >= 0);
    }

    CodeEditor::Coordinates CodeEditor::Coordinates::Invalid() noexcept
    {
        static Coordinates invalid{dont_use{}, dont_use{}};
        return invalid;
    }

    bool CodeEditor::Coordinates::operator==(const Coordinates& o) const noexcept
    {
        return (m_Line == o.m_Line) && (m_Column == o.m_Column);
    }

    bool CodeEditor::Coordinates::operator!=(const Coordinates& o) const noexcept
    {
        return (m_Line != o.m_Line) || (m_Column != o.m_Column);
    }

    bool CodeEditor::Coordinates::operator<(const Coordinates& o) const noexcept
    {
        if (m_Line != o.m_Line)
        {
            return m_Line < o.m_Line;
        }

        return m_Column < o.m_Column;
    }

    bool CodeEditor::Coordinates::operator>(const Coordinates& o) const noexcept
    {
        if (m_Line != o.m_Line)
        {
            return m_Line > o.m_Line;
        }

        return m_Column > o.m_Column;
    }

    bool CodeEditor::Coordinates::operator<=(const Coordinates& o) const noexcept
    {
        if (m_Line != o.m_Line)
        {
            return m_Line < o.m_Line;
        }

        return m_Column <= o.m_Column;
    }

    bool CodeEditor::Coordinates::operator>=(const Coordinates& o) const noexcept
    {
        if (m_Line != o.m_Line)
        {
            return m_Line > o.m_Line;
        }

        return m_Column >= o.m_Column;
    }

    CodeEditor::Coordinates::Coordinates(dont_use /*unused*/, dont_use /*unused*/) noexcept
        : m_Line{-1}
        , m_Column{-1}
    {}

    // Glyph

    CodeEditor::Glyph::Glyph(Char character, PaletteIndex color_index) noexcept
        : m_Char(character)
        , m_ColorIndex(color_index)
    {}

    // CodeEditor

    CodeEditor::CodeEditor(Emulator* emulator) noexcept
        : m_LineSpacing(1.0f)
        , m_UndoIndex(0u)
        , m_TabSize{static_cast<phi::uint_fast8_t>(4u)}
        , m_Overwrite(false)
        , m_ReadOnly(false)
        , m_WithinRender(false)
        , m_ScrollToCursor(false)
        , m_ScrollToTop(false)
        , m_TextChanged(false)
        , m_ColorizerEnabled(true)
        , m_TextStart(20.0f)
        , m_LeftMargin(10)
        , m_CursorPositionChanged(false)
        , m_ColorRangeMin(0)
        , m_ColorRangeMax(0)
        , m_SelectionMode(SelectionMode::Normal)
        , m_LastClick(-1.0f)
        , m_ShowWhitespaces(false)
        , m_StartTime(std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::system_clock::now().time_since_epoch())
                              .count())
        , m_PaletteBase(GetDarkPalette())
        , m_Palette()
        , m_Emulator(emulator)
    {
        Colorize();
        m_Lines.push_back(Line());
    }

    CodeEditor::~CodeEditor() noexcept
    {
        VerifyInternalState();
    }

    const CodeEditor::Palette& CodeEditor::GetPalette() const noexcept
    {
        return m_PaletteBase;
    }

    void CodeEditor::SetPalette(const Palette& value) noexcept
    {
        m_PaletteBase = value;
    }

    void CodeEditor::SetErrorMarkers(const ErrorMarkers& markers) noexcept
    {
        m_ErrorMarkers = markers;
    }

    void CodeEditor::AddErrorMarker(const phi::u32 line_number, const std::string& message) noexcept
    {
        if (m_ErrorMarkers.contains(line_number))
        {
            m_ErrorMarkers[line_number] += '\n' + message;
        }
        else
        {
            m_ErrorMarkers[line_number] = message;
        }
    }

    void CodeEditor::ClearErrorMarkers() noexcept
    {
        m_ErrorMarkers.clear();
    }

    CodeEditor::ErrorMarkers& CodeEditor::GetErrorMarkers() noexcept
    {
        return m_ErrorMarkers;
    }

    const CodeEditor::ErrorMarkers& CodeEditor::GetErrorMarkers() const noexcept
    {
        return m_ErrorMarkers;
    }

    void CodeEditor::SetBreakpoints(const Breakpoints& markers) noexcept
    {
        m_Breakpoints = markers;
    }

    phi::boolean CodeEditor::AddBreakpoint(const phi::u32 line_number) noexcept
    {
        return m_Breakpoints.insert(line_number).second;
    }

    phi::boolean CodeEditor::RemoveBreakpoint(const phi::u32 line_number) noexcept
    {
        auto iterator = m_Breakpoints.find(line_number);

        if (iterator != m_Breakpoints.end())
        {
            m_Breakpoints.erase(iterator);
            return true;
        }

        return false;
    }

    phi::boolean CodeEditor::ToggleBreakpoint(const phi::u32 line_number) noexcept
    {
        if (auto iterator = m_Breakpoints.find(line_number); iterator != m_Breakpoints.end())
        {
            m_Breakpoints.erase(iterator);
            return false;
        }

        AddBreakpoint(line_number);
        return true;
    }

    void CodeEditor::ClearBreakPoints() noexcept
    {
        m_Breakpoints.clear();
    }

    CodeEditor::Breakpoints& CodeEditor::GetBreakpoints() noexcept
    {
        return m_Breakpoints;
    }

    const CodeEditor::Breakpoints& CodeEditor::GetBreakpoints() const noexcept
    {
        return m_Breakpoints;
    }

    void CodeEditor::Render(const ImVec2& size, bool border) noexcept
    {
        // Verify that ImGui is correctly initialzied
        PHI_DBG_ASSERT(GImGui, "ImGui was not initialized!");

        m_WithinRender          = true;
        m_CursorPositionChanged = false;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(GetPaletteForIndex(
                                                        PaletteIndex::Background)));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        static constexpr const float min_size = 0.0f;
        static constexpr const float max_size =
                static_cast<float>(std::numeric_limits<int>::max()) * 0.95f;

        // Properly sanitize size
        ImVec2 sanitized_size;
        if (phi::is_nan(size.x) || std::isinf(size.x))
        {
            sanitized_size.x = min_size;
        }
        else
        {
            sanitized_size.x = phi::clamp(size.x, min_size, max_size);
        }

        if (phi::is_nan(size.y) || std::isinf(size.y))
        {
            sanitized_size.y = min_size;
        }
        else
        {
            sanitized_size.y = phi::clamp(size.y, min_size, max_size);
        }

        if (ImGui::Begin("Code Editor"))
        {
            ImGui::BeginChild("Code Editor", sanitized_size, border,
                              ImGuiWindowFlags_HorizontalScrollbar |
                                      ImGuiWindowFlags_AlwaysHorizontalScrollbar |
                                      ImGuiWindowFlags_NoMove);

            HandleKeyboardInputs();
            ImGui::PushAllowKeyboardFocus(true);

            HandleMouseInputs();

            if (m_TextChanged)
            {
                m_FullText = GetText();

                m_Emulator->ParseProgram(m_FullText);

                ColorizeInternal();

                ClearErrorMarkers();

                // Add error markers
                for (const dlx::ParseError& err : m_Emulator->m_DLXProgram.m_ParseErrors)
                {
                    AddErrorMarker(err.GetLineNumber(), err.ConstructMessage());
                }

                m_TextChanged = false;
            }

            InternalRender();

            ImGui::PopAllowKeyboardFocus();

            ImGui::EndChild();
        }

        ImGui::End();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        m_WithinRender = false;
    }

    void CodeEditor::SetText(const std::string& text) noexcept
    {
        m_Lines.clear();
        ResetState();
        m_Lines.emplace_back(Line());

        for (const char character : text)
        {
            if (character == '\r' ||
                (phi::is_control(character) && character != '\n' && character != '\t'))
            {
                // ignore the carriage return character and control characters
            }
            else if (character == '\n')
            {
                m_Lines.emplace_back(Line());
            }
            else
            {
                m_Lines.back().emplace_back(Glyph(character, PaletteIndex::Default));
            }
        }

        m_TextChanged = true;
        m_ScrollToTop = true;

        m_UndoBuffer.clear();
        m_UndoIndex = 0u;

        Colorize();
    }

    std::string CodeEditor::GetText() const noexcept
    {
        return GetText(Coordinates(), Coordinates(static_cast<phi::int32_t>(m_Lines.size()), 0));
    }

    void CodeEditor::ClearText() noexcept
    {
        PHI_DBG_ASSERT(!m_Lines.empty());

        if (IsReadOnly())
        {
            return;
        }

        // No text to clear
        if (m_Lines.size() == 1u && m_Lines[0u].empty())
        {
            return;
        }

        UndoRecord u;
        u.StoreBeforeState(this);

        u.m_Removed         = GetText();
        u.m_RemovedStart    = Coordinates(0, 0);
        phi::usize max_line = m_Lines.size() - 1u;
        u.m_RemovedEnd =
                Coordinates(static_cast<std::int32_t>(max_line.unsafe()),
                            GetLineMaxColumn(static_cast<phi::int32_t>(max_line.unsafe())));

        m_Lines.clear();
        m_Lines.emplace_back(Line{});
        ResetState();

        u.StoreAfterState(this);
        AddUndo(u);
    }

    void CodeEditor::SetTextLines(const std::vector<std::string>& lines) noexcept
    {
        m_Lines.clear();
        ResetState();

        if (lines.empty())
        {
            m_Lines.emplace_back(Line());
        }
        else
        {
            m_Lines.resize(lines.size());

            phi::usize line_number{0u};
            for (phi::usize index = 0u; index < lines.size(); ++index, ++line_number)
            {
                const std::string& line = lines[index.unsafe()];

                m_Lines[line_number.unsafe()].reserve(line.size());
                for (char character : line)
                {
                    if (character == '\n')
                    {
                        // Hang on an extra line at the end
                        m_Lines.emplace_back(Line{});

                        // Increase line number
                        line_number += 1u;
                    }
                    else
                    {
                        m_Lines[line_number.unsafe()].emplace_back(
                                Glyph(character, PaletteIndex::Default));
                    }
                }
            }
        }

        m_TextChanged = true;
        m_ScrollToTop = true;

        m_UndoBuffer.clear();
        m_UndoIndex = 0u;

        Colorize();
    }

    std::vector<std::string> CodeEditor::GetTextLines() const noexcept
    {
        std::vector<std::string> result;

        result.reserve(m_Lines.size());

        for (const Line& line : m_Lines)
        {
            std::string text;

            text.resize(line.size());

            for (size_t i = 0; i < line.size(); ++i)
            {
                text[i] = line[i].m_Char;
            }

            result.emplace_back(phi::move(text));
        }

        return result;
    }

    std::string CodeEditor::GetSelectedText() const noexcept
    {
        return GetText(m_State.m_SelectionStart, m_State.m_SelectionEnd);
    }

    std::string CodeEditor::GetCurrentLineText() const noexcept
    {
        const phi::i32 line_length = GetLineMaxColumn(m_State.m_CursorPosition.m_Line);

        return GetText(Coordinates(m_State.m_CursorPosition.m_Line, 0),
                       Coordinates(m_State.m_CursorPosition.m_Line, line_length));
    }

    phi::usize CodeEditor::GetTotalLines() const noexcept
    {
        return m_Lines.size();
    }

    void CodeEditor::SetOverwrite(bool overwrite) noexcept
    {
        m_Overwrite = overwrite;
    }

    void CodeEditor::ToggleOverwrite() noexcept
    {
        m_Overwrite = !m_Overwrite;
    }

    bool CodeEditor::IsOverwrite() const noexcept
    {
        return m_Overwrite;
    }

    void CodeEditor::SetReadOnly(bool value) noexcept
    {
        m_ReadOnly = value;
    }

    void CodeEditor::ToggleReadOnly() noexcept
    {
        m_ReadOnly = !m_ReadOnly;
    }

    bool CodeEditor::IsReadOnly() const noexcept
    {
        return m_ReadOnly;
    }

    bool CodeEditor::IsTextChanged() const noexcept
    {
        return m_TextChanged;
    }

    bool CodeEditor::IsCursorPositionChanged() const noexcept
    {
        return m_CursorPositionChanged;
    }

    void CodeEditor::SetColorizerEnable(bool value) noexcept
    {
        m_ColorizerEnabled = value;
    }

    void CodeEditor::ToggleColorizerEnabled() noexcept
    {
        m_ColorizerEnabled = !m_ColorizerEnabled;
    }

    bool CodeEditor::IsColorizerEnabled() const noexcept
    {
        return m_ColorizerEnabled;
    }

    CodeEditor::Coordinates CodeEditor::GetCursorPosition() const noexcept
    {
        return GetActualCursorCoordinates();
    }

    void CodeEditor::SetCursorPosition(const Coordinates& position) noexcept
    {
        // Sanitize
        const Coordinates new_pos = SanitizeCoordinates(position);

        if (m_State.m_CursorPosition != new_pos)
        {
            m_State.m_CursorPosition = new_pos;
            m_CursorPositionChanged  = true;
            EnsureCursorVisible();
        }
    }

    void CodeEditor::SetShowWhitespaces(bool value) noexcept
    {
        m_ShowWhitespaces = value;
    }

    void CodeEditor::ToggleShowWhitespaces() noexcept
    {
        m_ShowWhitespaces = !m_ShowWhitespaces;
    }

    bool CodeEditor::IsShowingWhitespaces() const noexcept
    {
        return m_ShowWhitespaces;
    }

    void CodeEditor::SetTabSize(phi::u8_fast new_tab_size) noexcept
    {
        new_tab_size = phi::clamp(new_tab_size, MinTabSize, MaxTabSize);

        if (new_tab_size != m_TabSize)
        {
            // Save old character indexes
            const phi::i32 cursor_char_index          = GetCharacterIndex(m_State.m_CursorPosition);
            const phi::i32 selection_start_char_index = GetCharacterIndex(m_State.m_SelectionStart);
            const phi::i32 selection_end_char_index   = GetCharacterIndex(m_State.m_SelectionEnd);

            // Update tab size
            m_TabSize = new_tab_size;

            // Set new character indexes
            m_State.m_CursorPosition.m_Column =
                    GetCharacterColumn(m_State.m_CursorPosition.m_Line, cursor_char_index);
            m_State.m_SelectionStart.m_Column =
                    GetCharacterColumn(m_State.m_SelectionStart.m_Line, selection_start_char_index);
            m_State.m_SelectionEnd.m_Column =
                    GetCharacterColumn(m_State.m_SelectionEnd.m_Line, selection_end_char_index);
        }
    }

    phi::u8_fast CodeEditor::GetTabSize() const noexcept
    {
        return m_TabSize;
    }

    void CodeEditor::EnterCharacter(ImWchar character, bool shift) noexcept
    {
        if (!IsReadOnly() && character != '\0' && IsValidUTF8Sequence(character))
        {
            EnterCharacterImpl(character, shift);
        }
    }

    void CodeEditor::Backspace() noexcept
    {
        if (!IsReadOnly())
        {
            BackspaceImpl();
        }
    }

    void CodeEditor::InsertText(const std::string& value) noexcept
    {
        InsertText(value.c_str());
    }

    void CodeEditor::InsertText(const char* value) noexcept
    {
        if (value == nullptr || m_ReadOnly)
        {
            return;
        }

        Coordinates pos         = GetActualCursorCoordinates();
        Coordinates start       = std::min(pos, m_State.m_SelectionStart);
        phi::i32    total_lines = pos.m_Line - start.m_Line;

        total_lines += InsertTextAt(pos, value);

        SetSelection(pos, pos);
        SetCursorPosition(pos);
        Colorize(start.m_Line - 1, total_lines + 2);
    }

    void CodeEditor::MoveUp(phi::u32 amount, bool select) noexcept
    {
        if (amount == 0u)
        {
            return;
        }

        const Coordinates old_pos = m_State.m_CursorPosition;
        m_State.m_CursorPosition.m_Line =
                phi::clamp(static_cast<phi::int32_t>(m_State.m_CursorPosition.m_Line.unsafe() -
                                                     amount.unsafe()),
                           0, static_cast<phi::int32_t>(m_Lines.size() - 1));

        // Move cursor to the beginning of the line after we reached the top
        if (amount.unsafe() > old_pos.m_Line.unsafe())
        {
            m_State.m_CursorPosition.m_Column = 0;
        }

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveDown(phi::u32 amount, bool select) noexcept
    {
        if (amount == 0u)
        {
            return;
        }

        PHI_DBG_ASSERT(m_State.m_CursorPosition.m_Column >= 0);

        const Coordinates old_pos = m_State.m_CursorPosition;
        m_State.m_CursorPosition.m_Line =
                phi::clamp(static_cast<phi::int32_t>(m_State.m_CursorPosition.m_Line.unsafe() +
                                                     amount.unsafe()),
                           0, static_cast<phi::int32_t>(m_Lines.size()) - 1);

        // Move cursor to the end of the line after we reached the bottom
        if (old_pos.m_Line.unsafe() == m_Lines.size() - 1u)
        {
            m_State.m_CursorPosition.m_Column = GetLineMaxColumn(m_State.m_CursorPosition.m_Line);
        }

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveLeft(phi::u32 amount, bool select, bool word_mode) noexcept
    {
        PHI_DBG_ASSERT(!m_Lines.empty());

        if (amount == 0u)
        {
            return;
        }

        const Coordinates old_pos = m_State.m_CursorPosition;
        m_State.m_CursorPosition  = GetActualCursorCoordinates();
        phi::i32 line             = m_State.m_CursorPosition.m_Line;
        phi::i32 cindex           = GetCharacterIndex(m_State.m_CursorPosition);

        while (amount > 0u)
        {
            amount -= 1u;

            if (cindex == 0)
            {
                if (line == 0)
                {
                    // We're already on the last line so we can't move further left
                    amount = 0u;
                }
                else
                {
                    line -= 1;
                    cindex = static_cast<phi::int32_t>(m_Lines[line.unsafe()].size());
                }
            }
            else
            {
                --cindex;
                if (cindex > 0 && static_cast<phi::int32_t>(m_Lines.size()) > line)
                {
                    while (cindex > 0 &&
                           IsUTFSequence(m_Lines[line.unsafe()][cindex.unsafe()].m_Char))
                    {
                        --cindex;
                    }
                }
            }

            m_State.m_CursorPosition = Coordinates(line, GetCharacterColumn(line, cindex));
            if (word_mode)
            {
                m_State.m_CursorPosition = FindWordStart(m_State.m_CursorPosition);
                cindex                   = GetCharacterIndex(m_State.m_CursorPosition);
            }
        }

        m_State.m_CursorPosition = Coordinates(line, GetCharacterColumn(line, cindex));
        PHI_DBG_ASSERT(m_State.m_CursorPosition.m_Column >= 0);

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveRight(phi::u32 amount, bool select, bool word_mode) noexcept
    {
        const Coordinates old_pos = m_State.m_CursorPosition;

        if (old_pos.m_Line.unsafe() >= m_Lines.size() || amount == 0u)
        {
            return;
        }

        phi::i32 cindex = GetCharacterIndex(m_State.m_CursorPosition);
        while (amount > 0u)
        {
            amount -= 1u;

            phi::i32 lindex = m_State.m_CursorPosition.m_Line;
            Line&    line   = m_Lines[lindex.unsafe()];

            if (cindex.unsafe() >= line.size())
            {
                if (m_State.m_CursorPosition.m_Line.unsafe() < m_Lines.size() - 1)
                {
                    m_State.m_CursorPosition.m_Line =
                            phi::clamp(m_State.m_CursorPosition.m_Line.unsafe() + 1, 0,
                                       static_cast<phi::int32_t>(m_Lines.size() - 1u));
                    m_State.m_CursorPosition.m_Column = 0;
                }
                else
                {
                    return;
                }
            }
            else
            {
                cindex += UTF8CharLength(line[cindex.unsafe()].m_Char);
                m_State.m_CursorPosition = Coordinates(lindex, GetCharacterColumn(lindex, cindex));

                if (word_mode)
                {
                    m_State.m_CursorPosition = FindNextWord(m_State.m_CursorPosition);
                }
            }
        }

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveTop(bool select) noexcept
    {
        const Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(Coordinates(0, 0));

        if (select)
        {
            m_InteractiveEnd   = old_pos;
            m_InteractiveStart = m_State.m_CursorPosition;
        }
        else
        {
            m_InteractiveStart = m_InteractiveEnd = m_State.m_CursorPosition;
        }

        SetSelection(m_InteractiveStart, m_InteractiveEnd);
        EnsureCursorVisible();
    }

    void CodeEditor::CodeEditor::MoveBottom(bool select) noexcept
    {
        const Coordinates old_pos = GetCursorPosition();

        const phi::i32 end_line = static_cast<phi::int32_t>(m_Lines.size() - 1u);
        Coordinates    new_pos  = Coordinates(end_line, GetLineMaxColumn(end_line));

        SetCursorPosition(new_pos);
        if (select)
        {
            m_InteractiveStart = old_pos;
            m_InteractiveEnd   = new_pos;
        }
        else
        {
            m_InteractiveStart = m_InteractiveEnd = new_pos;
        }

        SetSelection(m_InteractiveStart, m_InteractiveEnd);
        EnsureCursorVisible();
    }

    void CodeEditor::MoveHome(bool select) noexcept
    {
        const Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(Coordinates(m_State.m_CursorPosition.m_Line, 0));

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveEnd(bool select) noexcept
    {
        Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(
                Coordinates(m_State.m_CursorPosition.m_Line, GetLineMaxColumn(old_pos.m_Line)));

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::SetSelectionStart(const Coordinates& position) noexcept
    {
        m_State.m_SelectionStart = SanitizeCoordinates(position);
        if (m_State.m_SelectionStart > m_State.m_SelectionEnd)
        {
            phi::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
        }
    }

    void CodeEditor::SetSelectionEnd(const Coordinates& position) noexcept
    {
        m_State.m_SelectionEnd = SanitizeCoordinates(position);
        if (m_State.m_SelectionStart > m_State.m_SelectionEnd)
        {
            phi::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
        }
    }

    void CodeEditor::SetSelection(const Coordinates& start, const Coordinates& end,
                                  SelectionMode mode) noexcept
    {
        Coordinates old_sel_start = m_State.m_SelectionStart;
        Coordinates old_sel_end   = m_State.m_SelectionEnd;

        m_State.m_SelectionStart = SanitizeCoordinates(start);
        m_State.m_SelectionEnd   = SanitizeCoordinates(end);
        if (m_State.m_SelectionStart > m_State.m_SelectionEnd)
        {
            phi::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
        }

        switch (mode)
        {
            case CodeEditor::SelectionMode::Normal:
                break;
            case CodeEditor::SelectionMode::Word: {
                m_State.m_SelectionStart = FindWordStart(m_State.m_SelectionStart);
                if (!IsOnWordBoundary(m_State.m_SelectionEnd))
                {
                    m_State.m_SelectionEnd = FindWordEnd(FindWordStart(m_State.m_SelectionEnd));
                }
                break;
            }
            case CodeEditor::SelectionMode::Line: {
                const phi::i32   line_no   = m_State.m_SelectionEnd.m_Line;
                const phi::usize line_size = (phi::size_t)line_no.unsafe() < m_Lines.size() ?
                                                     m_Lines[line_no.unsafe()].size() :
                                                     0u;
                m_State.m_SelectionStart   = Coordinates(m_State.m_SelectionStart.m_Line, 0);
                m_State.m_SelectionEnd     = Coordinates(line_no, GetLineMaxColumn(line_no));
                break;
            }
#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_DBG_ASSERT_NOT_REACHED();
#endif
        }

        if (m_State.m_SelectionStart != old_sel_start || m_State.m_SelectionEnd != old_sel_end)
        {
            m_CursorPositionChanged = true;
        }
    }

    void CodeEditor::SelectWordUnderCursor() noexcept
    {
        Coordinates c = GetCursorPosition();
        SetSelection(FindWordStart(c), FindWordEnd(c));
    }

    void CodeEditor::SelectAll() noexcept
    {
        SetSelection(Coordinates(0, 0), Coordinates((phi::int32_t)m_Lines.size(), 0));
    }

    void CodeEditor::ClearSelection() noexcept
    {
        SetSelection(Coordinates(0, 0), Coordinates(0, 0));
    }

    bool CodeEditor::HasSelection() const noexcept
    {
        return m_State.m_SelectionEnd > m_State.m_SelectionStart;
    }

    CodeEditor::Coordinates CodeEditor::GetSelectionStart() const noexcept
    {
        return m_State.m_SelectionStart;
    }

    CodeEditor::Coordinates CodeEditor::GetSelectionEnd() const noexcept
    {
        return m_State.m_SelectionEnd;
    }

    void CodeEditor::Copy() const noexcept
    {
        if (HasSelection())
        {
            ImGui::SetClipboardText(GetSelectedText().c_str());
        }
        else
        {
            PHI_DBG_ASSERT(!m_Lines.empty());

            std::string str;
            const Line& line = m_Lines[GetActualCursorCoordinates().m_Line.unsafe()];

            for (const Glyph& g : line)
            {
                str.push_back(g.m_Char);
            }

            ImGui::SetClipboardText(str.c_str());
        }
    }

    void CodeEditor::Cut() noexcept
    {
        if (IsReadOnly())
        {
            Copy();
        }
        else
        {
            if (HasSelection())
            {
                UndoRecord u;
                u.StoreBeforeState(this);
                u.m_Removed      = GetSelectedText();
                u.m_RemovedStart = m_State.m_SelectionStart;
                u.m_RemovedEnd   = m_State.m_SelectionEnd;

                Copy();
                DeleteSelection();

                u.StoreAfterState(this);
                AddUndo(u);
            }
        }
    }

    void CodeEditor::Paste() noexcept
    {
        if (IsReadOnly())
        {
            return;
        }

        const char* clip_text = ImGui::GetClipboardText();
        PHI_DBG_ASSERT(clip_text);
        if (phi::string_length(clip_text) == 0u)
        {
            return;
        }

        UndoRecord u;
        u.StoreBeforeState(this);

        if (HasSelection())
        {
            u.m_Removed      = GetSelectedText();
            u.m_RemovedStart = m_State.m_SelectionStart;
            u.m_RemovedEnd   = m_State.m_SelectionEnd;
            DeleteSelection();
        }

        u.m_Added      = clip_text;
        u.m_AddedStart = GetActualCursorCoordinates();

        InsertText(clip_text);

        u.m_AddedEnd = GetActualCursorCoordinates();
        u.m_After    = m_State;
        AddUndo(u);
    }

    void CodeEditor::Delete() noexcept
    {
        PHI_DBG_ASSERT(!m_Lines.empty());

        if (m_ReadOnly)
        {
            return;
        }

        UndoRecord u;
        u.StoreBeforeState(this);

        if (HasSelection())
        {
            u.m_Removed      = GetSelectedText();
            u.m_RemovedStart = m_State.m_SelectionStart;
            u.m_RemovedEnd   = m_State.m_SelectionEnd;

            DeleteSelection();
        }
        else
        {
            Coordinates pos = GetActualCursorCoordinates();
            SetCursorPosition(pos);
            Line& line = m_Lines[pos.m_Line.unsafe()];

            if (pos.m_Column == GetLineMaxColumn(pos.m_Line))
            {
                if (pos.m_Line == static_cast<phi::int32_t>(m_Lines.size() - 1u))
                {
                    return;
                }

                u.m_Removed      = '\n';
                u.m_RemovedStart = u.m_RemovedEnd = GetActualCursorCoordinates();
                Advance(u.m_RemovedEnd);

                Line& next_line = m_Lines[pos.m_Line.unsafe() + 1];
                line.insert(line.end(), next_line.begin(), next_line.end());

                PHI_DBG_ASSERT(pos.m_Line.unsafe() <= m_Lines.size());
                RemoveLine(pos.m_Line + 1);
            }
            else
            {
                // Nothing todo when line is empty
                if (line.empty())
                {
                    return;
                }

                phi::i32 cindex = GetCharacterIndex(pos);
                PHI_DBG_ASSERT(cindex.unsafe() < line.size());

                Coordinates current_cursor_pos = GetActualCursorCoordinates();
                u.m_RemovedStart               = current_cursor_pos;
                u.m_RemovedEnd                 = current_cursor_pos;
                u.m_RemovedEnd.m_Column++;
                u.m_Removed = GetText(u.m_RemovedStart, u.m_RemovedEnd);

                phi::i32 length = UTF8CharLength(line[cindex.unsafe()].m_Char);
                while (length-- > 0 && cindex < (phi::int32_t)line.size())
                {
                    line.erase(line.begin() + cindex.unsafe());

                    // Correct selection state
                    if (m_State.m_SelectionStart.m_Line == current_cursor_pos.m_Line &&
                        m_State.m_SelectionStart.m_Column >= cindex &&
                        m_State.m_SelectionStart.m_Column > 0)
                    {
                        m_State.m_SelectionStart.m_Column -= 1;
                    }
                    if (m_State.m_SelectionEnd.m_Line == current_cursor_pos.m_Line &&
                        m_State.m_SelectionEnd.m_Column >= cindex &&
                        m_State.m_SelectionEnd.m_Column > 0)
                    {
                        m_State.m_SelectionEnd.m_Column -= 1;
                    }
                }
            }

            m_TextChanged = true;

            Colorize(pos.m_Line, 1);
        }

        u.StoreAfterState(this);
        AddUndo(u);
    }

    bool CodeEditor::CanUndo() const noexcept
    {
        return !m_ReadOnly && m_UndoIndex != 0u;
    }

    void CodeEditor::Undo(phi::u32 steps) noexcept
    {
        while (CanUndo() && steps > 0u)
        {
            m_UndoIndex -= 1u;
            m_UndoBuffer[m_UndoIndex.unsafe()].Undo(this);
            steps -= 1u;
        }
    }

    bool CodeEditor::CanRedo() const noexcept
    {
        return !m_ReadOnly && m_UndoIndex < m_UndoBuffer.size();
    }

    void CodeEditor::Redo(phi::u32 steps) noexcept
    {
        while (CanRedo() && steps > 0u)
        {
            m_UndoBuffer[m_UndoIndex.unsafe()].Redo(this);
            m_UndoIndex += 1u;
            steps -= 1u;
        }
    }

    std::string CodeEditor::GetEditorDump() const noexcept
    {
        std::string str;

        Coordinates cursor_pos = GetCursorPosition();

        str += "State:\n";
        str += fmt::format("Cursor position: {:d}, {:d}\n",
                           m_State.m_CursorPosition.m_Line.unsafe(),
                           m_State.m_CursorPosition.m_Column.unsafe());
        str += fmt::format("Actual cursor positon: {:d}, {:d}\n", cursor_pos.m_Line.unsafe(),
                           cursor_pos.m_Column.unsafe());
        str += fmt::format("Selection start: {:d}, {:d}\n",
                           m_State.m_SelectionStart.m_Line.unsafe(),
                           m_State.m_SelectionStart.m_Column.unsafe());
        str += fmt::format("Selection end: {:d}, {:d}\n", m_State.m_SelectionEnd.m_Line.unsafe(),
                           m_State.m_SelectionEnd.m_Column.unsafe());
        str += fmt::format("Has selection: {:s}\n", HasSelection() ? "true" : "false");

        str += "\n";
        str += "Options:\n";
        str += fmt::format("Line spacing: {:f}\n", m_LineSpacing);
        str += fmt::format("Tab size: {:d}\n", GetTabSize().unsafe());
        str += fmt::format("Overwrite: {:s}\n", IsOverwrite() ? "true" : "false");
        str += fmt::format("Read only: {:s}\n", IsReadOnly() ? "true" : "false");
        str += fmt::format("Show whitespaces: {:s}\n", IsShowingWhitespaces() ? "true" : "false");
        str += fmt::format("Selection mode: {:s}\n", magic_enum::enum_name(m_SelectionMode));

        const std::string              full_text = GetText();
        const std::vector<std::string> lines     = GetTextLines();
        std::string                    lines_text;

        for (const std::string& str : lines)
        {
            lines_text += str + '\n';
        }
        lines_text.pop_back();

        str += "\n";
        str += "Text:\n";
        str += fmt::format("Total lines: {:d}\n", GetTotalLines().unsafe());
        if (lines_text != full_text)
        {
            str += "[WARNING]: Lines and text don't match!";
            str += fmt::format("full_text:\n\"{:s}\"\n", full_text);

            str += "Lines:\n";
            for (phi::usize i{0u}; i < lines.size(); ++i)
            {
                str += fmt::format("{:02d}: \"{:s}\"\n", i.unsafe(), lines.at(i.unsafe()));
            }
        }
        else
        {
            str += fmt::format("\"{:s}\"\n", full_text);
        }
        if (HasSelection())
        {
            str += fmt::format("Selected text: \"{:s}\"\n", GetSelectedText());
        }
        else
        {
            str += "Selected text: N/A\n";
        }
        str += fmt::format("Current line text: \"{:s}\"\n", GetCurrentLineText());
        str += fmt::format("Word under cursor: \"{:s}\"\n", GetWordUnderCursor());

        str += "\n";
        str += "Error markers:\n";
        if (GetErrorMarkers().empty())
        {
            str += "None\n";
        }
        for (const auto& marker : GetErrorMarkers())
        {
            str += fmt::format("{:02d}: {:s}\n", marker.first.unsafe(), marker.second);
        }

        str += "\n";
        str += "Break points:\n";
        if (GetBreakpoints().empty())
        {
            str += "None\n";
        }
        for (const auto break_point : GetBreakpoints())
        {
            str += fmt::format("{:02d}\n", break_point.unsafe());
        }

        str += "\n";
        str += "Undo/Redo:\n";
        str += fmt::format("Can undo: {:s}\n", CanUndo() ? "true" : "false");
        str += fmt::format("Can redo: {:s}\n", CanRedo() ? "true" : "false");
        str += fmt::format("Undo index: {:d}\n", m_UndoIndex.unsafe());

        str += "UndoBuffer:\n";
        if (m_UndoBuffer.empty())
        {
            str += "Empty\n";
        }
        for (phi::usize i{0u}; i < m_UndoBuffer.size(); ++i)
        {
            const UndoRecord& record = m_UndoBuffer.at(i.unsafe());
            str += fmt::format("#{:02d} UndoRecord:\n", i.unsafe());

            if (!record.m_Added.empty())
            {
                str += fmt::format(
                        "Added: \"{:s}\" from {:d}, {:d} to {:d}, {:d}\n", record.m_Added,
                        record.m_AddedStart.m_Line.unsafe(), record.m_AddedStart.m_Column.unsafe(),
                        record.m_AddedEnd.m_Line.unsafe(), record.m_AddedEnd.m_Column.unsafe());
            }

            if (!record.m_Removed.empty())
            {
                str += fmt::format("Removed: \"{:s}\" from {:d}, {:d} to {:d}, {:d}\n",
                                   record.m_Removed, record.m_RemovedStart.m_Line.unsafe(),
                                   record.m_RemovedStart.m_Column.unsafe(),
                                   record.m_RemovedEnd.m_Line.unsafe(),
                                   record.m_RemovedEnd.m_Column.unsafe());
            }

            str += "State before:\n";
            str += fmt::format("Cursor position: {:d}, {:d}\n",
                               record.m_Before.m_CursorPosition.m_Line.unsafe(),
                               record.m_Before.m_CursorPosition.m_Column.unsafe());
            str += fmt::format("Selection start: {:d}, {:d}\n",
                               record.m_Before.m_SelectionStart.m_Line.unsafe(),
                               record.m_Before.m_SelectionStart.m_Column.unsafe());
            str += fmt::format("Selection end: {:d}, {:d}\n",
                               record.m_Before.m_SelectionEnd.m_Line.unsafe(),
                               record.m_Before.m_SelectionEnd.m_Column.unsafe());

            str += "State after:\n";
            str += fmt::format("Cursor position: {:d}, {:d}\n",
                               record.m_After.m_CursorPosition.m_Line.unsafe(),
                               record.m_After.m_CursorPosition.m_Column.unsafe());
            str += fmt::format("Selection start: {:d}, {:d}\n",
                               record.m_After.m_SelectionStart.m_Line.unsafe(),
                               record.m_After.m_SelectionStart.m_Column.unsafe());
            str += fmt::format("Selection end: {:d}, {:d}\n",
                               record.m_After.m_SelectionEnd.m_Line.unsafe(),
                               record.m_After.m_SelectionEnd.m_Column.unsafe());
        }

        return str;
    }

    void CodeEditor::VerifyInternalState() const noexcept
    {
        // Lines should never be empty
        PHI_DBG_ASSERT(!m_Lines.empty());

        // Verify Selection is still in a valid state
        PHI_DBG_ASSERT(m_State.m_SelectionEnd >= m_State.m_SelectionStart);
        PHI_DBG_ASSERT(m_State.m_SelectionStart.m_Line.unsafe() < m_Lines.size());
        PHI_DBG_ASSERT(m_State.m_SelectionStart.m_Column >= 0);
        PHI_DBG_ASSERT(m_State.m_SelectionEnd.m_Line.unsafe() < m_Lines.size());
        PHI_DBG_ASSERT(m_State.m_SelectionEnd.m_Column >= 0);

        // Verify cursor position
        PHI_DBG_ASSERT(m_State.m_CursorPosition.m_Line >= 0);
        PHI_DBG_ASSERT(m_State.m_CursorPosition.m_Line.unsafe() < m_Lines.size());
        PHI_DBG_ASSERT(m_State.m_CursorPosition.m_Column >= 0);

        // This should also always be true. But its implementation is way to slow when fuzzing
#if defined(DLXEMU_VERIFY_COLUMN)
        PHI_DBG_ASSERT(m_State.m_SelectionStart.m_Column <=
                       GetLineMaxColumn(m_State.m_SelectionStart.m_Line));
        PHI_DBG_ASSERT(m_State.m_SelectionEnd.m_Column <=
                       GetLineMaxColumn(m_State.m_SelectionEnd.m_Line));
#endif
    }

    const CodeEditor::Palette& CodeEditor::GetDarkPalette() noexcept
    {
        constexpr const static Palette p = {{
                0xff7f7f7f, // Default
                0xffd69c56, // OpCode
                0xff00ff00, // Register
                0xff7070e0, // IntegerLiteral
                0xff206020, // Comment
                0xff101010, // Background
                0xffe0e0e0, // Cursor
                0x80a06020, // Selection
                0x800020ff, // ErrorMarker
                0x40f08000, // Breakpoint
                0xff707000, // Line number
                0x40000000, // Current line fill
                0x40808080, // Current line fill (inactive)
                0x40a0a0a0, // Current line edge
        }};
        return p;
    }

    const CodeEditor::Palette& CodeEditor::GetLightPalette() noexcept
    {
        constexpr const static Palette p = {{
                0xff7f7f7f, // Default
                0xffff0c06, // OpCode
                0xff008000, // Register
                0xff2020a0, // IntegerLiteral
                0xff205020, // Comment
                0xffffffff, // Background
                0xff000000, // Cursor
                0x80600000, // Selection
                0xa00010ff, // ErrorMarker
                0x80f08000, // Breakpoint
                0xff505000, // Line number
                0x40000000, // Current line fill
                0x40808080, // Current line fill (inactive)
                0x40000000, // Current line edge
        }};
        return p;
    }

    const CodeEditor::Palette& CodeEditor::GetRetroBluePalette() noexcept
    {
        constexpr const static Palette p = {{
                0xff00ffff, // Default
                0xffffff00, // OpCode
                0xff00ff00, // Register
                0xff808000, // IntegerLiteral
                0xff808080, // Comment
                0xff800000, // Backgroung
                0xff0080ff, // Cursor
                0x80ffff00, // Selection
                0xa00000ff, // ErrorMarker
                0x80ff8000, // Breakpoint
                0xff808000, // Line number
                0x40000000, // Current line fill
                0x40808080, // Current line fill (inactive)
                0x40000000, // Current line edge
        }};
        return p;
    }

    // EditorState
    bool CodeEditor::EditorState::operator==(const EditorState& other) const noexcept
    {
        return other.m_CursorPosition == m_CursorPosition &&
               other.m_SelectionStart == m_SelectionStart && other.m_SelectionEnd == m_SelectionEnd;
    }

    bool CodeEditor::EditorState::operator!=(const EditorState& other) const noexcept
    {
        return other.m_CursorPosition != m_CursorPosition ||
               other.m_SelectionStart != m_SelectionStart || other.m_SelectionEnd != m_SelectionEnd;
    }

    // UndoRecord
    void CodeEditor::UndoRecord::Undo(CodeEditor* editor) const noexcept
    {
        PHI_DBG_ASSERT(editor != nullptr);

        if (!m_Added.empty())
        {
            editor->DeleteRange(m_AddedStart, m_AddedEnd);
            editor->Colorize(m_AddedStart.m_Line - 1, m_AddedEnd.m_Line - m_AddedStart.m_Line + 2);
        }

        if (!m_Removed.empty())
        {
            Coordinates start = m_RemovedStart;
            editor->InsertTextAt(start, m_Removed.c_str());
            editor->Colorize(m_RemovedStart.m_Line - 1,
                             m_RemovedEnd.m_Line - m_RemovedStart.m_Line + 2);
        }

        ApplyBeforeState(editor);
        editor->EnsureCursorVisible();
    }

    void CodeEditor::UndoRecord::Redo(CodeEditor* editor) const noexcept
    {
        PHI_DBG_ASSERT(editor != nullptr);

        if (!m_Removed.empty())
        {
            editor->DeleteRange(m_RemovedStart, m_RemovedEnd);
            editor->Colorize(m_RemovedStart.m_Line - 1,
                             m_RemovedEnd.m_Line - m_RemovedStart.m_Line + 1);
        }

        if (!m_Added.empty())
        {
            Coordinates start = m_AddedStart;
            editor->InsertTextAt(start, m_Added.c_str());
            editor->Colorize(m_AddedStart.m_Line - 1, m_AddedEnd.m_Line - m_AddedStart.m_Line + 1);
        }

        ApplyAfterState(editor);
        editor->EnsureCursorVisible();
    }

    void CodeEditor::UndoRecord::StoreBeforeState(CodeEditor* editor) noexcept
    {
        const Coordinates cursor_position = editor->GetCursorPosition();
        const Coordinates selection_start = editor->GetSelectionStart();
        const Coordinates selection_end   = editor->GetSelectionEnd();

        m_Before.m_CursorPosition = {cursor_position.m_Line,
                                     editor->GetCharacterIndex(cursor_position)};
        m_Before.m_SelectionStart = {selection_start.m_Line,
                                     editor->GetCharacterIndex(selection_start)};
        m_Before.m_SelectionEnd = {selection_end.m_Line, editor->GetCharacterIndex(selection_end)};
    }

    void CodeEditor::UndoRecord::StoreAfterState(CodeEditor* editor) noexcept
    {
        const Coordinates cursor_position = editor->GetCursorPosition();
        const Coordinates selection_start = editor->GetSelectionStart();
        const Coordinates selection_end   = editor->GetSelectionEnd();

        m_After.m_CursorPosition = {cursor_position.m_Line,
                                    editor->GetCharacterIndex(cursor_position)};
        m_After.m_SelectionStart = {selection_start.m_Line,
                                    editor->GetCharacterIndex(selection_start)};
        m_After.m_SelectionEnd   = {selection_end.m_Line, editor->GetCharacterIndex(selection_end)};
    }

    void CodeEditor::UndoRecord::ApplyBeforeState(CodeEditor* editor) const noexcept
    {
        Coordinates cursor_position = m_Before.m_CursorPosition;
        cursor_position.m_Column =
                editor->GetCharacterColumn(cursor_position.m_Line, cursor_position.m_Column);
        Coordinates selection_start = m_Before.m_SelectionStart;
        selection_start.m_Column =
                editor->GetCharacterColumn(selection_start.m_Line, selection_start.m_Column);
        Coordinates selection_end = m_Before.m_SelectionEnd;
        selection_end.m_Column =
                editor->GetCharacterColumn(selection_end.m_Line, selection_end.m_Column);

        editor->m_State.m_CursorPosition = cursor_position;
        editor->m_State.m_SelectionStart = selection_start;
        editor->m_State.m_SelectionEnd   = selection_end;
    }

    void CodeEditor::UndoRecord::ApplyAfterState(CodeEditor* editor) const noexcept
    {
        Coordinates cursor_position = m_After.m_CursorPosition;
        cursor_position.m_Column =
                editor->GetCharacterColumn(cursor_position.m_Line, cursor_position.m_Column);
        Coordinates selection_start = m_After.m_SelectionStart;
        selection_start.m_Column =
                editor->GetCharacterColumn(selection_start.m_Line, selection_start.m_Column);
        Coordinates selection_end = m_After.m_SelectionEnd;
        selection_end.m_Column =
                editor->GetCharacterColumn(selection_end.m_Line, selection_end.m_Column);

        editor->m_State.m_CursorPosition = cursor_position;
        editor->m_State.m_SelectionStart = selection_start;
        editor->m_State.m_SelectionEnd   = selection_end;
    }

    // CodeEditor - private
    void CodeEditor::Colorize(phi::i32 from_line, phi::i32 count) noexcept
    {
        const phi::i32 to_line = count == -1 ?
                                         (phi::int32_t)m_Lines.size() :
                                         phi::min((phi::int32_t)m_Lines.size(), from_line + count);

        m_ColorRangeMin = phi::min(m_ColorRangeMin, from_line);
        m_ColorRangeMax = phi::max(m_ColorRangeMax, to_line);
        m_ColorRangeMin = phi::max(0, m_ColorRangeMin);
        m_ColorRangeMax = phi::max(m_ColorRangeMin, m_ColorRangeMax);
    }

    float CodeEditor::TextDistanceToLineStart(const Coordinates& from) const noexcept
    {
        const Line& line     = m_Lines[from.m_Line.unsafe()];
        float       distance = 0.0f;
        float       space_size =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr)
                        .x;
        phi::i32 col_index = GetCharacterIndex(from);
        for (size_t it = 0u; it < line.size() && it < col_index.unsafe();)
        {
            if (line[it].m_Char == '\t')
            {
                distance = (1.0f + std::floor((1.0f + distance) /
                                              (float(m_TabSize.unsafe()) * space_size))) *
                           (float(m_TabSize.unsafe()) * space_size);
                ++it;
            }
            else
            {
                phi::i32 d = UTF8CharLength(line[it].m_Char);
                char     temp_c_string[7];
                phi::i32 i = 0;
                for (; i < 6 && d-- > 0 && it < static_cast<phi::int32_t>(line.size()); ++i, ++it)
                {
                    temp_c_string[i.unsafe()] = line[it].m_Char;
                }

                temp_c_string[i.unsafe()] = '\0';
                distance += ImGui::GetFont()
                                    ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f,
                                                    temp_c_string, nullptr, nullptr)
                                    .x;
            }
        }

        return distance;
    }

    void CodeEditor::EnsureCursorVisible() noexcept
    {
        if (!m_WithinRender)
        {
            m_ScrollToCursor = true;
            return;
        }

        float scroll_x = ImGui::GetScrollX();
        float scroll_y = ImGui::GetScrollY();

        float height = ImGui::GetWindowHeight();
        float width  = ImGui::GetWindowWidth();

        phi::i32 top    = 1 + (phi::int32_t)std::ceil(scroll_y / m_CharAdvance.y);
        phi::i32 bottom = (phi::int32_t)std::ceil((scroll_y + height) / m_CharAdvance.y);

        phi::i32 left  = (phi::int32_t)std::ceil(scroll_x / m_CharAdvance.x);
        phi::i32 right = (phi::int32_t)std::ceil((scroll_x + width) / m_CharAdvance.x);

        Coordinates pos = GetActualCursorCoordinates();
        float       len = TextDistanceToLineStart(pos);

        if (pos.m_Line < top)
        {
            ImGui::SetScrollY(phi::max(0.0f, (pos.m_Line - 1).unsafe() * m_CharAdvance.y));
        }
        if (pos.m_Line > bottom - 4)
        {
            ImGui::SetScrollY(phi::max(0.0f, (pos.m_Line + 4).unsafe() * m_CharAdvance.y - height));
        }
        if (len + m_TextStart < (left + 4).unsafe())
        {
            ImGui::SetScrollX(phi::max(0.0f, len + m_TextStart - 4));
        }
        if (len + m_TextStart > (right - 4).unsafe())
        {
            ImGui::SetScrollX(phi::max(0.0f, len + m_TextStart + 4 - width));
        }
    }

    phi::i32 CodeEditor::GetPageSize() const noexcept
    {
        float height = ImGui::GetWindowHeight() - 20.0f;
        return (phi::int32_t)std::floor(height / m_CharAdvance.y);
    }

    std::string CodeEditor::GetText(const Coordinates& start, const Coordinates& end) const noexcept
    {
        std::string result;

        phi::i32   lstart = start.m_Line;
        phi::i32   lend   = end.m_Line;
        phi::i32   istart = GetCharacterIndex(start);
        phi::i32   iend   = GetCharacterIndex(end);
        phi::usize s      = 0u;

        PHI_DBG_ASSERT(lstart.unsafe() < m_Lines.size());

        for (phi::i32 i = lstart; i < lend; ++i)
        {
            s += m_Lines[i.unsafe()].size();
        }

        result.reserve((s + s / 8u).unsafe());

        while (istart < iend || lstart < lend)
        {
            PHI_DBG_ASSERT(lstart < static_cast<phi::int32_t>(m_Lines.size()));

            const Line& line = m_Lines[lstart.unsafe()];
            if (istart < static_cast<phi::int32_t>(line.size()))
            {
                result += line[istart.unsafe()].m_Char;
                istart++;
            }
            else
            {
                istart = 0;
                ++lstart;

                if (lstart.unsafe() != m_Lines.size())
                {
                    result += '\n';
                }
            }
        }

        return result;
    }

    CodeEditor::Coordinates CodeEditor::GetActualCursorCoordinates() const noexcept
    {
        return SanitizeCoordinates(m_State.m_CursorPosition);
    }

    CodeEditor::Coordinates CodeEditor::SanitizeCoordinates(const Coordinates& value) const noexcept
    {
        phi::i32 line   = value.m_Line;
        phi::i32 column = value.m_Column;

        if (line >= static_cast<phi::int32_t>(m_Lines.size()))
        {
            PHI_DBG_ASSERT(!m_Lines.empty());

            line   = static_cast<phi::int32_t>(m_Lines.size() - 1u);
            column = GetLineMaxColumn(line);

            return {line, column};
        }
        // For negative lines simply return 0, 0
        if (line < 0)
        {
            return {0, 0};
        }

        PHI_DBG_ASSERT(!m_Lines.empty());
        PHI_DBG_ASSERT(line.unsafe() < m_Lines.size());
        PHI_DBG_ASSERT(line >= 0);

        // Sanitize column
        const Line& current_line = m_Lines[line.unsafe()];
        phi::i32    new_column   = 0;
        for (phi::i32 char_index{0}; char_index < static_cast<phi ::int32_t>(current_line.size());)
        {
            if (new_column >= column)
            {
                break;
            }

            const char current_char = static_cast<char>(current_line[char_index.unsafe()].m_Char);
            if (current_char == '\t')
            {
                new_column += GetTabSizeAt(new_column);
            }
            else
            {
                ++new_column;
            }

            char_index += UTF8CharLength(current_char);
        }
        PHI_DBG_ASSERT(new_column >= 0);
        PHI_DBG_ASSERT(new_column <= GetLineMaxColumn(line));

        column = new_column;

        // Sanitize line
        line = phi::max(0, line);

        return {line, column};
    }

    void CodeEditor::Advance(Coordinates& coordinates) const noexcept
    {
        if (coordinates.m_Line >= static_cast<phi::int32_t>(m_Lines.size()))
        {
            return;
        }

        const Line& line   = m_Lines[coordinates.m_Line.unsafe()];
        phi::i32    cindex = GetCharacterIndex(coordinates);

        if (cindex + 1 < static_cast<phi::int32_t>(line.size()))
        {
            phi::i32 delta = UTF8CharLength(line[cindex.unsafe()].m_Char);
            cindex         = phi::min(cindex + delta, static_cast<phi::int32_t>(line.size() - 1));
        }
        else
        {
            ++coordinates.m_Line;
            cindex = 0;
        }
        coordinates.m_Column = GetCharacterColumn(coordinates.m_Line, cindex);
    }

    void CodeEditor::DeleteRange(const Coordinates& start, const Coordinates& end) noexcept
    {
        PHI_DBG_ASSERT(end > start);
        PHI_DBG_ASSERT(!m_ReadOnly);
        PHI_DBG_ASSERT(start.m_Line < static_cast<phi::int32_t>(m_Lines.size()));
        PHI_DBG_ASSERT(end.m_Line < static_cast<phi::int32_t>(m_Lines.size()));

        phi::i32 start_index = GetCharacterIndex(start);
        phi::i32 end_index   = GetCharacterIndex(end);

        if (start.m_Line == end.m_Line)
        {
            Line&          line       = m_Lines[start.m_Line.unsafe()];
            const phi::i32 max_column = GetLineMaxColumn(start.m_Line);

            if (end.m_Column >= max_column)
            {
                line.erase(line.begin() + start_index.unsafe(), line.end());

                // Fix selection
                if (m_State.m_SelectionStart.m_Column > start_index)
                {
                    m_State.m_SelectionStart.m_Column = start_index;
                }
                if (m_State.m_SelectionEnd.m_Column > start_index)
                {
                    m_State.m_SelectionEnd.m_Column = start_index;
                }
            }
            else
            {
                phi::i32 start_column = GetCharacterColumn(start.m_Line, start_index);
                phi::i32 end_column   = GetCharacterColumn(end.m_Line, end_index);

                line.erase(line.begin() + start_index.unsafe(), line.begin() + end_index.unsafe());

                // Fix selection
                if (m_State.m_SelectionStart.m_Column > start_column)
                {
                    if (m_State.m_SelectionStart.m_Column <= end_column)
                    {
                        m_State.m_SelectionStart.m_Column = start_column;
                    }
                    else
                    {
                        m_State.m_SelectionStart.m_Column -= (end_column - start_column);
                    }
                }
                if (m_State.m_SelectionEnd.m_Column > start_column)
                {
                    if (m_State.m_SelectionEnd.m_Column <= end_column)
                    {
                        m_State.m_SelectionEnd.m_Column = start_column;
                    }
                    else
                    {
                        m_State.m_SelectionEnd.m_Column -= (end_column - start_column);
                    }
                }
            }
        }
        else
        {
            Line& first_line = m_Lines[start.m_Line.unsafe()];
            Line& last_line  = m_Lines[end.m_Line.unsafe()];

            first_line.erase(first_line.begin() + start_index.unsafe(), first_line.end());
            last_line.erase(last_line.begin(), last_line.begin() + end_index.unsafe());

            if (start.m_Line < end.m_Line)
            {
                first_line.insert(first_line.end(), last_line.begin(), last_line.end());
            }

            if (start.m_Line < end.m_Line)
            {
                RemoveLine(start.m_Line + 1, end.m_Line + 1);
            }
        }

        m_TextChanged = true;
    }

    phi::i32 CodeEditor::InsertTextAt(Coordinates& /* inout */ where, const char* value) noexcept
    {
        PHI_DBG_ASSERT(!m_ReadOnly);

        phi::i32 cindex      = GetCharacterIndex(where);
        phi::i32 total_lines = 0;
        while (*value != '\0')
        {
            PHI_DBG_ASSERT(!m_Lines.empty());

            if (*value == '\n')
            {
                if (cindex < (phi::int32_t)m_Lines[where.m_Line.unsafe()].size())
                {
                    Line& new_line = InsertLine(where.m_Line + 1);
                    Line& line     = m_Lines[where.m_Line.unsafe()];
                    new_line.insert(new_line.begin(), line.begin() + cindex.unsafe(), line.end());
                    line.erase(line.begin() + cindex.unsafe(), line.end());
                }
                else
                {
                    InsertLine(where.m_Line + 1);
                }

                ++where.m_Line;
                where.m_Column = 0;
                cindex         = 0;
                ++total_lines;
                ++value;
            }
            else if (*value == '\t')
            {
                Line& line = m_Lines[where.m_Line.unsafe()];

                line.insert(line.begin() + cindex.unsafe(), Glyph(*value++, PaletteIndex::Default));
                cindex += 1;

                where.m_Column += GetTabSizeAt(where.m_Column);
            }
            else
            {
                Line&    line = m_Lines[where.m_Line.unsafe()];
                phi::i32 d    = UTF8CharLength(*value);

                while (d-- > 0 && *value != '\0')
                {
                    line.insert(line.begin() + cindex.unsafe(),
                                Glyph(*value++, PaletteIndex::Default));
                    cindex += 1;
                }

                ++where.m_Column;
            }

            m_TextChanged = true;
        }

        return total_lines;
    }

    void CodeEditor::AddUndo(UndoRecord& value) noexcept
    {
        PHI_DBG_ASSERT(!m_ReadOnly);

#if defined(DLXEMU_VERIFY_UNDO_REDO)
        VerifyInternalState();
        // Reject empty undos
        PHI_DBG_ASSERT(!(value.m_Added.empty() && value.m_Removed.empty()));
        // Start and end are valid
        PHI_DBG_ASSERT(value.m_AddedStart <= value.m_AddedEnd);
        PHI_DBG_ASSERT(value.m_RemovedStart <= value.m_RemovedEnd);
#endif

        m_UndoBuffer.resize((m_UndoIndex + 1u).unsafe());
        m_UndoBuffer.back() = value;
        ++m_UndoIndex;

#if defined(DLXEMU_VERIFY_UNDO_REDO)
        VerifyInternalState();

        PHI_DBG_ASSERT(CanUndo());

        const std::string text_before          = GetText();
        EditorState       state_before         = m_State;
        state_before.m_CursorPosition.m_Column = GetCharacterIndex(state_before.m_CursorPosition);
        state_before.m_SelectionStart.m_Column = GetCharacterIndex(state_before.m_SelectionStart);
        state_before.m_SelectionEnd.m_Column   = GetCharacterIndex(state_before.m_SelectionEnd);

        // Test the undo
        Undo();
        VerifyInternalState();

        const std::string text_after_undo  = GetText();
        EditorState       state_after_undo = m_State;
        state_after_undo.m_CursorPosition.m_Column =
                GetCharacterIndex(state_after_undo.m_CursorPosition);
        state_after_undo.m_SelectionStart.m_Column =
                GetCharacterIndex(state_after_undo.m_SelectionStart);
        state_after_undo.m_SelectionEnd.m_Column =
                GetCharacterIndex(state_after_undo.m_SelectionEnd);

        PHI_DBG_ASSERT(CanRedo());

        // Test the redo
        Redo();
        VerifyInternalState();

        const std::string text_after          = GetText();
        EditorState       state_after         = m_State;
        state_after.m_CursorPosition.m_Column = GetCharacterIndex(state_after.m_CursorPosition);
        state_after.m_SelectionStart.m_Column = GetCharacterIndex(state_after.m_SelectionStart);
        state_after.m_SelectionEnd.m_Column   = GetCharacterIndex(state_after.m_SelectionEnd);

        PHI_DBG_ASSERT(text_before == text_after);
        PHI_DBG_ASSERT(state_before == state_after);
#endif
    }

    CodeEditor::Coordinates CodeEditor::ScreenPosToCoordinates(
            const ImVec2& position) const noexcept
    {
        ImVec2 origin = ImGui::GetCursorScreenPos();
        ImVec2 local(position.x - origin.x, position.y - origin.y);

        phi::i32 line_no = std::max(0, (phi::int32_t)floor(local.y / m_CharAdvance.y));

        phi::i32 column_coord = 0;

        if (line_no >= 0 && line_no < (phi::int32_t)m_Lines.size())
        {
            const Line& line = m_Lines[line_no.unsafe()];

            phi::i32 column_index = 0;
            float    column_x     = 0.0f;

            while (column_index < static_cast<phi::int32_t>(line.size()))
            {
                float column_width = 0.0f;

                if (line[column_index.unsafe()].m_Char == '\t')
                {
                    float space_size =
                            ImGui::GetFont()
                                    ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ")
                                    .x;
                    float old_x = column_x;
                    float new_column_x =
                            (1.0f + std::floor((1.0f + column_x) /
                                               (float(m_TabSize.unsafe()) * space_size))) *
                            (float(m_TabSize.unsafe()) * space_size);
                    column_width = new_column_x - old_x;

                    if (m_TextStart + column_x + column_width * 0.5f > local.x)
                    {
                        break;
                    }

                    column_x = new_column_x;
                    column_coord =
                            (column_coord.unsafe() / m_TabSize.unsafe()) * m_TabSize.unsafe() +
                            m_TabSize.unsafe();
                    column_index += 1;
                }
                else
                {
                    char     buf[7];
                    phi::i32 d = UTF8CharLength(line[column_index.unsafe()].m_Char);
                    phi::i32 i = 0;
                    while (i < 6 && d-- > 0)
                    {
                        buf[i.unsafe()] = line[column_index.unsafe()].m_Char;
                        i += 1;
                        column_index += 1;
                    }
                    buf[i.unsafe()] = '\0';
                    column_width =
                            ImGui::GetFont()
                                    ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf)
                                    .x;
                    if (m_TextStart + column_x + column_width * 0.5f > local.x)
                    {
                        break;
                    }
                    column_x += column_width;
                    column_coord++;
                }
            }
        }

        return SanitizeCoordinates(Coordinates(line_no, column_coord));
    }

    CodeEditor::Coordinates CodeEditor::FindWordStart(const Coordinates& from) const noexcept
    {
        if (from.m_Line >= static_cast<phi::int32_t>(m_Lines.size()))
        {
            return from;
        }

        const Line& line   = m_Lines[from.m_Line.unsafe()];
        phi::i32    cindex = GetCharacterIndex(from);

        if (cindex >= static_cast<phi::int32_t>(line.size()))
        {
            return from;
        }

        while (cindex > 0 && phi::is_space(line[cindex.unsafe()].m_Char))
        {
            --cindex;
        }

        const PaletteIndex cstart = line[cindex.unsafe()].m_ColorIndex;
        while (cindex > 0)
        {
            Glyph glyph = line[cindex.unsafe()];
            if ((glyph.m_Char & 0xC0) != 0x80) // not UTF code sequence 10xxxxxx
            {
                if (glyph.m_Char <= 32 && phi::is_space(glyph.m_Char))
                {
                    cindex += 1;
                    break;
                }

                if (cstart != line[phi::size_t(cindex.unsafe() - 1)].m_ColorIndex)
                {
                    break;
                }
            }

            cindex -= 1;
        }

        return {from.m_Line, GetCharacterColumn(from.m_Line, cindex)};
    }

    CodeEditor::Coordinates CodeEditor::FindWordEnd(const Coordinates& from) const noexcept
    {
        Coordinates at = from;
        if (at.m_Line >= (phi::int32_t)m_Lines.size())
        {
            return at;
        }

        const Line& line   = m_Lines[at.m_Line.unsafe()];
        phi::i32    cindex = GetCharacterIndex(at);

        if (cindex >= (phi::int32_t)line.size())
        {
            return at;
        }

        bool         prevspace = phi::is_space(line[cindex.unsafe()].m_Char);
        PaletteIndex cstart    = line[cindex.unsafe()].m_ColorIndex;
        while (cindex < static_cast<phi::int32_t>(line.size()))
        {
            Glyph    c = line[cindex.unsafe()];
            phi::i32 d = UTF8CharLength(c.m_Char);

            if (cstart != line[cindex.unsafe()].m_ColorIndex)
            {
                break;
            }

            if (prevspace != phi::is_space(c.m_Char))
            {
                if (phi::is_space(c.m_Char))
                {
                    while (cindex < (phi::int32_t)line.size() &&
                           phi::is_space(line[cindex.unsafe()].m_Char))
                    {
                        cindex += 1;
                    }
                }
                break;
            }

            cindex += d;
        }

        return {from.m_Line, GetCharacterColumn(from.m_Line, cindex)};
    }

    CodeEditor::Coordinates CodeEditor::FindNextWord(const Coordinates& from) const noexcept
    {
        Coordinates at = from;
        if (at.m_Line >= (phi::int32_t)m_Lines.size())
        {
            return at;
        }

        // skip to the next non-word character
        phi::i32 cindex  = GetCharacterIndex(from);
        bool     is_word = false;
        bool     skip    = false;
        if (cindex < (phi::int32_t)m_Lines[at.m_Line.unsafe()].size())
        {
            const Line& line = m_Lines[at.m_Line.unsafe()];
            is_word          = phi::is_alpha_numeric(line[cindex.unsafe()].m_Char);
            skip             = is_word;
        }

        while (!is_word || skip)
        {
            if (at.m_Line >= static_cast<phi::int32_t>(m_Lines.size()))
            {
                phi::i32 l = phi::max(0, (phi::int32_t)m_Lines.size() - 1);

                return {l, GetLineMaxColumn(l)};
            }

            const Line& line = m_Lines[at.m_Line.unsafe()];
            if (cindex < (phi::int32_t)line.size())
            {
                is_word = phi::is_alpha_numeric(line[cindex.unsafe()].m_Char);

                if (is_word && !skip)
                {
                    return {at.m_Line, GetCharacterColumn(at.m_Line, cindex)};
                }

                if (!is_word)
                {
                    skip = false;
                }

                cindex++;
            }
            else
            {
                cindex = 0;
                at.m_Line += 1;
                skip    = false;
                is_word = false;
            }
        }

        return at;
    }

    std::string CodeEditor::GetWordUnderCursor() const noexcept
    {
        Coordinates coordinates = GetCursorPosition();
        return GetWordAt(coordinates);
    }

    std::string CodeEditor::GetWordAt(const Coordinates& coords) const noexcept
    {
        Coordinates start = FindWordStart(coords);
        Coordinates end   = FindWordEnd(coords);

        std::string result;

        phi::i32 istart = GetCharacterIndex(start);
        phi::i32 iend   = GetCharacterIndex(end);

        for (phi::i32 it = istart; it < iend; ++it)
        {
            result.push_back(m_Lines[coords.m_Line.unsafe()][it.unsafe()].m_Char);
        }

        return result;
    }

    phi::i32 CodeEditor::GetCharacterIndex(const Coordinates& coordinates) const noexcept
    {
        if (coordinates.m_Line >= static_cast<phi::int32_t>(m_Lines.size()))
        {
            return -1;
        }

        const Line& line = m_Lines[coordinates.m_Line.unsafe()];
        phi::i32    c    = 0;
        phi::i32    i    = 0;
        for (; i < static_cast<phi::int32_t>(line.size()) && c < coordinates.m_Column;)
        {
            if (line[i.unsafe()].m_Char == '\t')
            {
                c = (c.unsafe() / m_TabSize.unsafe()) * m_TabSize.unsafe() + m_TabSize.unsafe();
            }
            else
            {
                ++c;
            }

            i += UTF8CharLength(line[i.unsafe()].m_Char);
        }

        return i;
    }

    phi::i32 CodeEditor::GetCharacterColumn(phi::i32 line_number, phi::i32 index) const noexcept
    {
        PHI_DBG_ASSERT(line_number < static_cast<phi::int32_t>(m_Lines.size()));

        const Line& line = m_Lines[line_number.unsafe()];
        phi::i32    col  = 0;
        phi::i32    i    = 0;

        while (i < index && i < static_cast<phi::int32_t>(line.size()))
        {
            char c = line[i.unsafe()].m_Char;
            i += UTF8CharLength(c);
            if (c == '\t')
            {
                col = (col.unsafe() / m_TabSize.unsafe()) * m_TabSize.unsafe() + m_TabSize.unsafe();
            }
            else
            {
                col++;
            }
        }

        return col;
    }

    phi::i32 CodeEditor::GetLineCharacterCount(phi::i32 line_number) const noexcept
    {
        if (line_number >= static_cast<phi::int32_t>(m_Lines.size()))
        {
            return 0;
        }

        const Line& line  = m_Lines[line_number.unsafe()];
        phi::i32    count = 0;

        for (phi::i32 i{0}; i < static_cast<phi::int32_t>(line.size()); ++count)
        {
            i += UTF8CharLength(line[i.unsafe()].m_Char);
        }

        return count;
    }

    phi::i32 CodeEditor::GetLineMaxColumn(phi::i32 line_number) const noexcept
    {
        if (line_number >= static_cast<phi::int32_t>(m_Lines.size()))
        {
            return 0;
        }

        const Line& line = m_Lines[line_number.unsafe()];
        phi::i32    col  = 0;

        for (phi::i32 i = 0; i < static_cast<phi::int32_t>(line.size());)
        {
            char c = line[i.unsafe()].m_Char;
            if (c == '\t')
            {
                col = (col.unsafe() / m_TabSize.unsafe()) * m_TabSize.unsafe() + m_TabSize.unsafe();
            }
            else
            {
                col++;
            }

            i += UTF8CharLength(c);
        }

        return col;
    }

    bool CodeEditor::IsOnWordBoundary(const Coordinates& at) const noexcept
    {
        if (at.m_Line >= (phi::int32_t)m_Lines.size() || at.m_Column == 0)
        {
            return true;
        }

        const Line& line   = m_Lines[at.m_Line.unsafe()];
        phi::i32    cindex = GetCharacterIndex(at);
        if (cindex >= (phi::int32_t)line.size())
        {
            return true;
        }

        if (m_ColorizerEnabled)
        {
            return line[cindex.unsafe()].m_ColorIndex !=
                   line[phi::size_t(cindex.unsafe() - 1)].m_ColorIndex;
        }

        return phi::is_space(line[cindex.unsafe()].m_Char) !=
               phi::is_space(line[cindex.unsafe() - 1].m_Char);
    }

    void CodeEditor::RemoveLine(phi::i32 start, phi::i32 end) noexcept
    {
        PHI_DBG_ASSERT(!m_ReadOnly);
        PHI_DBG_ASSERT(end >= start);
        PHI_DBG_ASSERT(m_Lines.size() > (phi::size_t)((end - start).unsafe()));

        // Remove error markers
        ErrorMarkers etmp;
        for (const auto& marker : m_ErrorMarkers)
        {
            ErrorMarkers::value_type e(marker.first.unsafe() >= start.unsafe() ?
                                               marker.first.unsafe() - 1 :
                                               marker.first,
                                       marker.second);
            if (e.first.unsafe() >= start.unsafe() && e.first.unsafe() <= end.unsafe())
            {
                continue;
            }
            etmp.insert(e);
        }
        m_ErrorMarkers = phi::move(etmp);

        // Remove breakpoints
        Breakpoints btmp;
        for (const auto line_number : m_Breakpoints)
        {
            if (line_number.unsafe() >= start.unsafe() && line_number.unsafe() <= end.unsafe())
            {
                continue;
            }
            btmp.insert(line_number.unsafe() >= start.unsafe() ? line_number.unsafe() - 1 :
                                                                 line_number.unsafe());
        }
        m_Breakpoints = phi::move(btmp);

        m_Lines.erase(m_Lines.begin() + start.unsafe(), m_Lines.begin() + end.unsafe());
        PHI_DBG_ASSERT(!m_Lines.empty());

        // Fix selection state
        if (m_State.m_SelectionStart.m_Line >= start)
        {
            m_State.m_SelectionStart.m_Line -= end - start;
        }
        if (m_State.m_SelectionEnd.m_Line >= start)
        {
            m_State.m_SelectionEnd.m_Line -= end - start;
        }
        if (m_State.m_SelectionStart > m_State.m_SelectionEnd)
        {
            phi::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
        }

        m_TextChanged = true;
    }

    void CodeEditor::RemoveLine(phi::i32 index) noexcept
    {
        PHI_DBG_ASSERT(!m_ReadOnly);
        PHI_DBG_ASSERT(m_Lines.size() > 1);

        // Clear error markers on that line
        ErrorMarkers etmp;
        for (const auto& marker : m_ErrorMarkers)
        {
            ErrorMarkers::value_type e(marker.first.unsafe() > index.unsafe() ?
                                               marker.first.unsafe() - 1 :
                                               marker.first,
                                       marker.second);
            if (e.first.unsafe() - 1 == index.unsafe())
            {
                continue;
            }
            etmp.insert(e);
        }
        m_ErrorMarkers = phi::move(etmp);

        // Remove breakpoints on that line
        Breakpoints btmp;
        for (const auto line_number : m_Breakpoints)
        {
            if (line_number.unsafe() == index.unsafe())
            {
                continue;
            }

            btmp.insert(line_number.unsafe() >= index.unsafe() ? line_number.unsafe() - 1 :
                                                                 line_number);
        }
        m_Breakpoints = phi::move(btmp);

        // Fix selection
        if (m_State.m_SelectionStart.m_Line >= index)
        {
            m_State.m_SelectionStart.m_Line--;
        }
        if (m_State.m_SelectionEnd.m_Line >= index)
        {
            m_State.m_SelectionEnd.m_Line--;
        }

        m_Lines.erase(m_Lines.begin() + index.unsafe());
        PHI_DBG_ASSERT(!m_Lines.empty());

        m_TextChanged = true;
    }

    CodeEditor::Line& CodeEditor::InsertLine(phi::i32 index) noexcept
    {
        PHI_DBG_ASSERT(!m_ReadOnly);

        Line& result = *m_Lines.insert(m_Lines.begin() + index.unsafe(), Line());

        ErrorMarkers etmp;
        for (const auto& marker : m_ErrorMarkers)
        {
            etmp.insert(ErrorMarkers::value_type(marker.first.unsafe() >= index.unsafe() ?
                                                         marker.first.unsafe() + 1 :
                                                         marker.first,
                                                 marker.second));
        }
        m_ErrorMarkers = phi::move(etmp);

        Breakpoints btmp;
        for (const phi::u32 line_number : m_Breakpoints)
        {
            btmp.insert(line_number.unsafe() >= index.unsafe() ? line_number.unsafe() + 1 :
                                                                 line_number);
        }
        m_Breakpoints = phi::move(btmp);

        return result;
    }

    void CodeEditor::EnterCharacterImpl(ImWchar character, bool shift) noexcept
    {
        PHI_DBG_ASSERT(!m_ReadOnly);
        PHI_DBG_ASSERT(IsValidUTF8Sequence(character));

        UndoRecord u;
        u.StoreBeforeState(this);

        if (HasSelection())
        {
            // Do indenting
            if (character == '\t' &&
                (m_State.m_SelectionStart.m_Column == 0 ||
                 m_State.m_SelectionStart.m_Line != m_State.m_SelectionEnd.m_Line))
            {
                Coordinates start        = m_State.m_SelectionStart;
                Coordinates end          = m_State.m_SelectionEnd;
                Coordinates original_end = end;

                PHI_DBG_ASSERT(start < end);
                start.m_Column = 0;
                //          end.mColumn = end.mLine < mLines.size() ? mLines[end.mLine].size() : 0;
                if (end.m_Column == 0 && end.m_Line > 0)
                {
                    --end.m_Line;
                }
                if (end.m_Line >= (phi::int32_t)m_Lines.size())
                {
                    PHI_DBG_ASSERT(!m_Lines.empty());
                    end.m_Line = (phi::int32_t)m_Lines.size() - 1;
                }
                end.m_Column = GetLineMaxColumn(end.m_Line);

                //if (end.mColumn >= GetLineMaxColumn(end.mLine))
                //  end.mColumn = GetLineMaxColumn(end.mLine) - 1;

                u.m_RemovedStart = start;
                u.m_RemovedEnd   = end;
                u.m_Removed      = GetText(start, end);

                bool modified = false;

                for (phi::i32 line_index = start.m_Line; line_index <= end.m_Line; ++line_index)
                {
                    Line& line = m_Lines[line_index.unsafe()];
                    if (shift)
                    {
                        // Remove idention
                        if (line.empty())
                        {
                            // Skip already empty lines
                            continue;
                        }

                        if (line.front().m_Char == '\t')
                        {
                            line.erase(line.begin());
                            modified = true;
                        }
                        else
                        {
                            for (int32_t j = 0;
                                 j < m_TabSize && !line.empty() && line.front().m_Char == ' '; j++)
                            {
                                line.erase(line.begin());
                                modified = true;
                            }
                        }
                    }
                    else
                    {
                        // Add indention
                        line.insert(line.begin(), Glyph('\t', PaletteIndex::Background));
                        modified = true;
                    }
                }

                if (modified)
                {
                    start = Coordinates(start.m_Line, GetCharacterColumn(start.m_Line, 0));
                    Coordinates range_end;

                    if (original_end.m_Column != 0)
                    {
                        end       = Coordinates(end.m_Line, GetLineMaxColumn(end.m_Line));
                        range_end = end;
                        u.m_Added = GetText(start, end);
                    }
                    else
                    {
                        end       = Coordinates(original_end.m_Line, 0);
                        range_end = Coordinates(end.m_Line - 1, GetLineMaxColumn(end.m_Line - 1));
                        u.m_Added = GetText(start, range_end);
                    }

                    u.m_AddedStart           = start;
                    u.m_AddedEnd             = range_end;
                    m_State.m_SelectionStart = start;
                    m_State.m_SelectionEnd   = end;

                    u.StoreAfterState(this);
                    AddUndo(u);

                    m_TextChanged = true;

                    EnsureCursorVisible();
                }

                return;
            } // c == '\t'
            else
            {
                u.m_Removed      = GetSelectedText();
                u.m_RemovedStart = m_State.m_SelectionStart;
                u.m_RemovedEnd   = m_State.m_SelectionEnd;
                DeleteSelection();
            }
        } // HasSelection

        Coordinates coord = GetActualCursorCoordinates();
        u.m_AddedStart    = coord;

        PHI_DBG_ASSERT(!m_Lines.empty());

        if (character == '\n')
        {
            InsertLine(coord.m_Line + 1);
            u.m_Added = static_cast<char>(character);

            Line& line     = m_Lines[coord.m_Line.unsafe()];
            Line& new_line = m_Lines[coord.m_Line.unsafe() + 1];

            for (phi::i32 it = 0; it < static_cast<phi::int32_t>(line.size()) &&
                                  it < coord.m_Column && phi::is_blank(line[it.unsafe()].m_Char);
                 ++it)
            {
                new_line.push_back(line[it.unsafe()]);
                u.m_Added += static_cast<char>(line[it.unsafe()].m_Char);
            }

            const phi::usize whitespace_size = new_line.size();
            phi::i32         cindex          = GetCharacterIndex(coord);
            new_line.insert(new_line.end(), line.begin() + cindex.unsafe(), line.end());
            line.erase(line.begin() + cindex.unsafe(), line.begin() + line.size());
            SetCursorPosition(Coordinates(
                    coord.m_Line + 1, GetCharacterColumn(coord.m_Line.unsafe() + 1,
                                                         (phi::int32_t)whitespace_size.unsafe())));

            // Fix selection
            if (!HasSelection())
            {
                ClearSelection();
            }
        }
        else
        {
            phi::array<char, 5u> buffer{};
            const phi::u8_fast   length = ImTextCharToUtf8(buffer, character);

            // We require a valid ut8 sequence
            PHI_DBG_ASSERT(length > 0u);

            Line&    line   = m_Lines[coord.m_Line.unsafe()];
            phi::i32 cindex = GetCharacterIndex(coord);

            if (m_Overwrite && cindex < (phi::int32_t)line.size())
            {
                phi::i32 d = UTF8CharLength(line[cindex.unsafe()].m_Char);

                u.m_RemovedStart = m_State.m_CursorPosition;
                u.m_RemovedEnd =
                        Coordinates(coord.m_Line, GetCharacterColumn(coord.m_Line, cindex + d));

                while (d-- > 0 && cindex < (phi::int32_t)line.size())
                {
                    u.m_Removed += static_cast<char>(line[cindex.unsafe()].m_Char);
                    line.erase(line.begin() + cindex.unsafe());
                }
            }

            for (char* pointer = buffer.begin(); *pointer != '\0'; ++pointer, ++cindex)
            {
                line.insert(line.begin() + cindex.unsafe(), Glyph(*pointer, PaletteIndex::Default));
            }
            u.m_Added = buffer.begin();

            SetCursorPosition(Coordinates(coord.m_Line, GetCharacterColumn(coord.m_Line, cindex)));
        }

        m_TextChanged = true;

        u.m_AddedEnd = GetActualCursorCoordinates();
        u.StoreAfterState(this);

        AddUndo(u);

        Colorize(coord.m_Line - 1, 3);
        EnsureCursorVisible();
    }

    void CodeEditor::BackspaceImpl() noexcept
    {
        PHI_DBG_ASSERT(!m_ReadOnly);
        PHI_DBG_ASSERT(!m_Lines.empty());

        UndoRecord u;
        u.StoreBeforeState(this);

        if (HasSelection())
        {
            u.m_Removed      = GetSelectedText();
            u.m_RemovedStart = m_State.m_SelectionStart;
            u.m_RemovedEnd   = m_State.m_SelectionEnd;

            DeleteSelection();
        }
        else
        {
            Coordinates pos = GetActualCursorCoordinates();
            SetCursorPosition(pos);

            if (m_State.m_CursorPosition.m_Column == 0)
            {
                if (m_State.m_CursorPosition.m_Line == 0)
                {
                    return;
                }

                u.m_Removed      = '\n';
                u.m_RemovedStart = u.m_RemovedEnd =
                        Coordinates(pos.m_Line - 1, GetLineMaxColumn(pos.m_Line - 1));
                Advance(u.m_RemovedEnd);

                Line&    line      = m_Lines[m_State.m_CursorPosition.m_Line.unsafe()];
                Line&    prev_line = m_Lines[m_State.m_CursorPosition.m_Line.unsafe() - 1];
                phi::i32 prev_size = GetLineMaxColumn(m_State.m_CursorPosition.m_Line - 1);
                prev_line.insert(prev_line.end(), line.begin(), line.end());

                ErrorMarkers etmp;
                for (auto& i : m_ErrorMarkers)
                {
                    etmp.insert(ErrorMarkers::value_type(
                            i.first.unsafe() - 1 == m_State.m_CursorPosition.m_Line.unsafe() ?
                                    i.first.unsafe() - 1 :
                                    i.first,
                            i.second));
                }
                m_ErrorMarkers = phi::move(etmp);

                RemoveLine(m_State.m_CursorPosition.m_Line);
                --m_State.m_CursorPosition.m_Line;
                m_State.m_CursorPosition.m_Column = prev_size;
            }
            else
            {
                Line&    line   = m_Lines[m_State.m_CursorPosition.m_Line.unsafe()];
                phi::i32 cindex = GetCharacterIndex(pos) - 1;

                if (line[cindex.unsafe()].m_Char == '\t')
                {
                    u.m_RemovedStart.m_Line = GetActualCursorCoordinates().m_Line;
                    u.m_RemovedEnd          = GetActualCursorCoordinates();
                    u.m_Removed             = '\t';

                    line.erase(line.begin() + cindex.unsafe());

                    // Move cursor back
                    m_State.m_CursorPosition.m_Column = GetCharacterColumn(pos.m_Line, cindex);
                    u.m_RemovedStart.m_Column         = GetCharacterColumn(pos.m_Line, cindex);
                }
                else
                {
                    phi::i32 cend = cindex + 1;
                    while (cindex > 0 && IsUTFSequence(line[cindex.unsafe()].m_Char))
                    {
                        --cindex;
                    }

                    u.m_RemovedStart = u.m_RemovedEnd = GetActualCursorCoordinates();
                    --u.m_RemovedStart.m_Column;
                    --m_State.m_CursorPosition.m_Column;

                    while (cindex < static_cast<phi::int32_t>(line.size()) && cend-- > cindex)
                    {
                        u.m_Removed += line[cindex.unsafe()].m_Char;
                        line.erase(line.begin() + cindex.unsafe());
                    }
                }
            }

            m_TextChanged = true;

            EnsureCursorVisible();
            Colorize(m_State.m_CursorPosition.m_Line, 1);
        }

        // Correct selection
        m_State.m_SelectionStart = SanitizeCoordinates(m_State.m_SelectionStart);
        m_State.m_SelectionEnd   = SanitizeCoordinates(m_State.m_SelectionEnd);

        u.StoreAfterState(this);
        AddUndo(u);
    }

    void CodeEditor::DeleteSelection() noexcept
    {
        if (m_State.m_SelectionEnd == m_State.m_SelectionStart)
        {
            return;
        }

        DeleteRange(m_State.m_SelectionStart, m_State.m_SelectionEnd);

        SetSelection(m_State.m_SelectionStart, m_State.m_SelectionStart);
        SetCursorPosition(m_State.m_SelectionStart);
        Colorize(m_State.m_SelectionStart.m_Line, 1);
        m_TextChanged = true;
    }

    ImU32 CodeEditor::GetGlyphColor(const Glyph& glyph) const noexcept
    {
        if (!m_ColorizerEnabled)
        {
            return GetPaletteForIndex(PaletteIndex::Default);
        }

        return GetPaletteForIndex(glyph.m_ColorIndex);
    }

    void CodeEditor::HandleKeyboardInputs() noexcept
    {
        if (!ImGui::IsWindowFocused())
        {
            return;
        }

        ImGuiIO& io    = ImGui::GetIO();
        bool     shift = io.KeyShift;
        bool     ctrl  = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
        bool     alt   = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

        if (ImGui::IsWindowHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
        }
        //ImGui::CaptureKeyboardFromApp(true);

        io.WantCaptureKeyboard = true;
        io.WantTextInput       = true;

        if (!IsReadOnly() && ctrl && !shift && !alt &&
            ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
        {
            Undo();
        }
        else if (!IsReadOnly() && !ctrl && !shift && alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
        {
            Undo();
        }
        else if (!IsReadOnly() && ctrl && !shift && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Y)))
        {
            Redo();
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
        {
            MoveUp(1u, shift);
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
        {
            MoveDown(1u, shift);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
        {
            MoveLeft(1u, shift, ctrl);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
        {
            MoveRight(1u, shift, ctrl);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp)))
        {
            MoveUp(GetPageSize().unsafe() - 4u, shift);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown)))
        {
            MoveDown(GetPageSize().unsafe() - 4u, shift);
        }
        else if (!alt && ctrl && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
        {
            MoveTop(shift);
        }
        else if (ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
        {
            MoveBottom(shift);
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Home)))
        {
            MoveHome(shift);
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_End)))
        {
            MoveEnd(shift);
        }
        else if (!IsReadOnly() && !ctrl && !shift && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
        {
            Delete();
        }
        else if (!IsReadOnly() && !ctrl && !shift && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Backspace)))
        {
            BackspaceImpl();
        }
        else if (!ctrl && !shift && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
        {
            m_Overwrite ^= true;
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
        {
            Copy();
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_C)))
        {
            Copy();
        }
        else if (!IsReadOnly() && !ctrl && shift && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
        {
            Paste();
        }
        else if (!IsReadOnly() && ctrl && !shift && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_V)))
        {
            Paste();
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_X)))
        {
            Cut();
        }
        else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
        {
            Cut();
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
        {
            SelectAll();
        }
        else if (!IsReadOnly() && !ctrl && !shift && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)))
        {
            EnterCharacterImpl('\n', false);
        }
        else if (!IsReadOnly() && !ctrl && !alt &&
                 ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
        {
            EnterCharacterImpl('\t', shift);
        }

        if (!IsReadOnly() && !io.InputQueueCharacters.empty())
        {
            for (phi::i32 i{0}; i < io.InputQueueCharacters.Size; ++i)
            {
                ImWchar c = io.InputQueueCharacters[i.unsafe()];
                if (c != 0 && (c == '\n' || c >= 32))
                {
                    EnterCharacterImpl(c, shift);
                }
            }

            io.InputQueueCharacters.resize(0);
        }
    }

    void CodeEditor::HandleMouseInputs() noexcept
    {
        ImGuiIO& io    = ImGui::GetIO();
        bool     shift = io.KeyShift;
        bool     ctrl  = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
        bool     alt   = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

        if (ImGui::IsWindowHovered())
        {
            if (!shift && !alt)
            {
                bool   click        = ImGui::IsMouseClicked(0);
                bool   double_click = ImGui::IsMouseDoubleClicked(0);
                double t            = ImGui::GetTime();
                bool   triple_click =
                        click && !double_click &&
                        (m_LastClick != -1.0f && (t - m_LastClick) < io.MouseDoubleClickTime);

                /*
                    Left mouse button triple click
                */

                if (triple_click)
                {
                    if (!ctrl)
                    {
                        m_State.m_CursorPosition = m_InteractiveStart = m_InteractiveEnd =
                                ScreenPosToCoordinates(ImGui::GetMousePos());
                        m_SelectionMode = SelectionMode::Line;
                        SetSelection(m_InteractiveStart, m_InteractiveEnd, m_SelectionMode);
                    }

                    m_LastClick = -1.0f;
                }

                /*
                    Left mouse button double click
                */

                else if (double_click)
                {
                    if (!ctrl)
                    {
                        m_State.m_CursorPosition = m_InteractiveStart = m_InteractiveEnd =
                                ScreenPosToCoordinates(ImGui::GetMousePos());
                        if (m_SelectionMode == SelectionMode::Line)
                        {
                            m_SelectionMode = SelectionMode::Normal;
                        }
                        else
                        {
                            m_SelectionMode = SelectionMode::Word;
                        }
                        SetSelection(m_InteractiveStart, m_InteractiveEnd, m_SelectionMode);
                    }

                    m_LastClick = (float)ImGui::GetTime();
                }

                /*
                    Left mouse button click
                */
                else if (click)
                {
                    m_State.m_CursorPosition = m_InteractiveStart = m_InteractiveEnd =
                            ScreenPosToCoordinates(ImGui::GetMousePos());
                    if (ctrl)
                    {
                        m_SelectionMode = SelectionMode::Word;
                    }
                    else
                    {
                        m_SelectionMode = SelectionMode::Normal;
                    }
                    SetSelection(m_InteractiveStart, m_InteractiveEnd, m_SelectionMode);

                    m_LastClick = (float)ImGui::GetTime();
                }
                // Mouse left button dragging (=> update selection)
                else if (ImGui::IsMouseDragging(0) && ImGui::IsMouseDown(0))
                {
                    io.WantCaptureMouse      = true;
                    m_State.m_CursorPosition = m_InteractiveEnd =
                            ScreenPosToCoordinates(ImGui::GetMousePos());
                    SetSelection(m_InteractiveStart, m_InteractiveEnd, m_SelectionMode);
                }
            }
        }
    }

    void CodeEditor::InternalRender() noexcept
    {
        /* Compute m_CharAdvance regarding to scaled font size (Ctrl + mouse wheel)*/
        const float font_size =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr)
                        .x;
        m_CharAdvance = ImVec2(font_size, ImGui::GetTextLineHeightWithSpacing() * m_LineSpacing);

        /* Update palette with the current alpha from style */
        for (phi::usize i = 0u; i < (phi::size_t)PaletteIndex::Max; ++i)
        {
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(m_PaletteBase[i]);
            color.w *= ImGui::GetStyle().Alpha;
            m_Palette[i] = ImGui::ColorConvertFloat4ToU32(color);
        }

        PHI_DBG_ASSERT(m_LineBuffer.empty());

        ImVec2      content_size = ImGui::GetWindowContentRegionMax();
        ImDrawList* draw_list    = ImGui::GetWindowDrawList();
        float       longest(m_TextStart);

        if (m_ScrollToTop)
        {
            m_ScrollToTop = false;
            ImGui::SetScrollY(0.f);
        }

        ImVec2 cursor_screen_pos = ImGui::GetCursorScreenPos();
        float  scroll_x          = ImGui::GetScrollX();
        float  scroll_y          = ImGui::GetScrollY();

        phi::i32 line_no         = (phi::int32_t)std::floor(scroll_y / m_CharAdvance.y);
        phi::i32 global_line_max = (phi::int32_t)m_Lines.size();
        phi::i32 line_max =
                phi::clamp((phi::int32_t)m_Lines.size() - 1, 0,
                           line_no.unsafe() + (phi::int32_t)std::floor((scroll_y + content_size.y) /
                                                                       m_CharAdvance.y));

        // Deduce m_TextStart by evaluating mLines size (global lineMax) plus two spaces as text width
        char buf[16];
        snprintf(buf, 16, " %d ", global_line_max.unsafe());
        m_TextStart =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr)
                        .x +
                m_LeftMargin.unsafe();

        PHI_DBG_ASSERT(!m_Lines.empty());
        float space_size =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr)
                        .x;

        while (line_no <= line_max)
        {
            ImVec2 line_start_screen_pos = ImVec2(
                    cursor_screen_pos.x, cursor_screen_pos.y + line_no.unsafe() * m_CharAdvance.y);
            ImVec2 text_screen_pos =
                    ImVec2(line_start_screen_pos.x + m_TextStart, line_start_screen_pos.y);

            Line& line            = m_Lines[line_no.unsafe()];
            longest               = phi::max(m_TextStart + TextDistanceToLineStart(Coordinates(
                                                                   line_no, GetLineMaxColumn(line_no))),
                                             longest);
            phi::i32    column_no = 0;
            Coordinates line_start_coord(line_no, 0);
            Coordinates line_end_coord(line_no, GetLineMaxColumn(line_no));

            // Draw selection for the current line
            float sstart = -1.0f;
            float ssend  = -1.0f;

            PHI_DBG_ASSERT(m_State.m_SelectionStart <= m_State.m_SelectionEnd);
            if (m_State.m_SelectionStart <= line_end_coord)
            {
                sstart = m_State.m_SelectionStart > line_start_coord ?
                                 TextDistanceToLineStart(m_State.m_SelectionStart) :
                                 0.0f;
            }

            if (m_State.m_SelectionEnd > line_start_coord)
            {
                ssend = TextDistanceToLineStart(m_State.m_SelectionEnd < line_end_coord ?
                                                        m_State.m_SelectionEnd :
                                                        line_end_coord);
            }

            if (m_State.m_SelectionEnd.m_Line > line_no)
            {
                ssend += m_CharAdvance.x;
            }

            if (sstart != -1 && ssend != -1 && sstart < ssend)
            {
                ImVec2 vstart(line_start_screen_pos.x + m_TextStart + sstart,
                              line_start_screen_pos.y);
                ImVec2 vend(line_start_screen_pos.x + m_TextStart + ssend,
                            line_start_screen_pos.y + m_CharAdvance.y);

                draw_list->AddRectFilled(vstart, vend, GetPaletteForIndex(PaletteIndex::Selection));
            }

            // Draw breakpoints
            ImVec2 start = ImVec2(line_start_screen_pos.x + scroll_x, line_start_screen_pos.y);

            if (m_Breakpoints.contains((phi::uint32_t)(line_no + 1).unsafe()))
            {
                ImVec2 end = ImVec2(line_start_screen_pos.x + content_size.x + 2.0f * scroll_x,
                                    line_start_screen_pos.y + m_CharAdvance.y);
                draw_list->AddRectFilled(start, end, GetPaletteForIndex(PaletteIndex::Breakpoint));
            }

            // Draw error markers
            auto error_it = m_ErrorMarkers.find((phi::uint32_t)(line_no + 1).unsafe());
            if (error_it != m_ErrorMarkers.end())
            {
                ImVec2 end = ImVec2(line_start_screen_pos.x + content_size.x + 2.0f * scroll_x,
                                    line_start_screen_pos.y + m_CharAdvance.y);
                draw_list->AddRectFilled(start, end, GetPaletteForIndex(PaletteIndex::ErrorMarker));

                if (GImGui->HoveredWindow == ImGui::GetCurrentWindow() &&
                    ImGui::IsMouseHoveringRect(line_start_screen_pos, end))
                {
                    ImGui::BeginTooltip();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    ImGui::Text("Error at line %d:", error_it->first.unsafe());
                    ImGui::PopStyleColor();
                    ImGui::Separator();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
                    ImGui::Text("%s", error_it->second.c_str());
                    ImGui::PopStyleColor();
                    ImGui::EndTooltip();
                }
            }

            // Draw line number (right aligned)
            snprintf(buf, 16, "%d  ", line_no.unsafe() + 1);

            float line_no_width = ImGui::GetFont()
                                          ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf,
                                                          nullptr, nullptr)
                                          .x;
            draw_list->AddText(ImVec2(line_start_screen_pos.x + m_TextStart - line_no_width,
                                      line_start_screen_pos.y),
                               GetPaletteForIndex(PaletteIndex::LineNumber), buf);

            if (m_State.m_CursorPosition.m_Line == line_no)
            {
                bool focused = ImGui::IsWindowFocused();

                // Highlight the current line (where the cursor is)
                if (!HasSelection())
                {
                    ImVec2 end =
                            ImVec2(start.x + content_size.x + scroll_x, start.y + m_CharAdvance.y);
                    draw_list->AddRectFilled(
                            start, end,
                            GetPaletteForIndex(focused ? PaletteIndex::CurrentLineFill :
                                                         PaletteIndex::CurrentLineFillInactive));
                    draw_list->AddRect(start, end,
                                       GetPaletteForIndex(PaletteIndex::CurrentLineEdge), 1.0f);
                }

                // Render the cursor
                if (focused)
                {
                    std::int64_t time_end =
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                    std::chrono::system_clock::now().time_since_epoch())
                                    .count();
                    std::uint64_t elapsed = time_end - m_StartTime;
                    if (elapsed > 400)
                    {
                        float    width  = 1.0f;
                        phi::i32 cindex = GetCharacterIndex(m_State.m_CursorPosition);
                        float    cx     = TextDistanceToLineStart(m_State.m_CursorPosition);

                        if (m_Overwrite && cindex < (phi::int32_t)line.size())
                        {
                            char c = line[cindex.unsafe()].m_Char;
                            if (c == '\t')
                            {
                                float x = (1.0f +
                                           std::floor((1.0f + cx) /
                                                      (float(m_TabSize.unsafe()) * space_size))) *
                                          (float(m_TabSize.unsafe()) * space_size);
                                width = x - cx;
                            }
                            else
                            {
                                char buf2[2];
                                buf2[0] = line[cindex.unsafe()].m_Char;
                                buf2[1] = '\0';
                                width   = ImGui::GetFont()
                                                ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX,
                                                                -1.0f, buf2)
                                                .x;
                            }
                        }
                        ImVec2 cstart(text_screen_pos.x + cx, line_start_screen_pos.y);
                        ImVec2 cend(text_screen_pos.x + cx + width,
                                    line_start_screen_pos.y + m_CharAdvance.y);
                        draw_list->AddRectFilled(cstart, cend,
                                                 GetPaletteForIndex(PaletteIndex::Cursor));
                        if (elapsed > 800)
                        {
                            m_StartTime = time_end;
                        }
                    }
                }
            }

            // Render colorized text
            ImU32  prev_color = line.empty() ? GetPaletteForIndex(PaletteIndex::Default) :
                                               GetGlyphColor(line[0]);
            ImVec2 buffer_offset;

            for (phi::i32 i{0}; i < static_cast<phi::int32_t>(line.size());)
            {
                CodeEditor::Glyph& glyph = line[i.unsafe()];
                ImU32              color = GetGlyphColor(glyph);

                if ((color != prev_color || glyph.m_Char == '\t' || glyph.m_Char == ' ') &&
                    !m_LineBuffer.empty())
                {
                    const ImVec2 new_offset(text_screen_pos.x + buffer_offset.x,
                                            text_screen_pos.y + buffer_offset.y);
                    draw_list->AddText(new_offset, prev_color, m_LineBuffer.c_str());
                    ImVec2 text_size =
                            ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f,
                                                            m_LineBuffer.c_str(), nullptr, nullptr);
                    buffer_offset.x += text_size.x;
                    m_LineBuffer.clear();
                }
                prev_color = color;

                if (glyph.m_Char == '\t')
                {
                    float old_x = buffer_offset.x;
                    buffer_offset.x =
                            (1.0f + std::floor((1.0f + buffer_offset.x) /
                                               (float(m_TabSize.unsafe()) * space_size))) *
                            (float(m_TabSize.unsafe()) * space_size);
                    ++i;

                    if (m_ShowWhitespaces)
                    {
                        const float  s  = ImGui::GetFontSize();
                        const float  x1 = text_screen_pos.x + old_x + 1.0f;
                        const float  x2 = text_screen_pos.x + buffer_offset.x - 1.0f;
                        const float  y  = text_screen_pos.y + buffer_offset.y + s * 0.5f;
                        const ImVec2 p1(x1, y);
                        const ImVec2 p2(x2, y);
                        const ImVec2 p3(x2 - s * 0.2f, y - s * 0.2f);
                        const ImVec2 p4(x2 - s * 0.2f, y + s * 0.2f);

                        draw_list->AddLine(p1, p2, 0x90909090);
                        draw_list->AddLine(p2, p3, 0x90909090);
                        draw_list->AddLine(p2, p4, 0x90909090);
                    }
                }
                else if (glyph.m_Char == ' ')
                {
                    if (m_ShowWhitespaces)
                    {
                        const float s = ImGui::GetFontSize();
                        const float x = text_screen_pos.x + buffer_offset.x + space_size * 0.5f;
                        const float y = text_screen_pos.y + buffer_offset.y + s * 0.5f;
                        draw_list->AddCircleFilled(ImVec2(x, y), 1.5f, 0x80808080, 4);
                    }
                    buffer_offset.x += space_size;
                    i++;
                }
                else
                {
                    phi::i32 l = UTF8CharLength(glyph.m_Char);
                    while (l-- > 0)
                    {
                        m_LineBuffer.push_back(line[i.unsafe()].m_Char);
                        i += 1;
                    }
                }
                ++column_no;
            }
            if (!m_LineBuffer.empty())
            {
                const ImVec2 new_offset(text_screen_pos.x + buffer_offset.x,
                                        text_screen_pos.y + buffer_offset.y);
                draw_list->AddText(new_offset, prev_color, m_LineBuffer.c_str());
                m_LineBuffer.clear();
            }

            ++line_no;
        }

        // Draw a tooltip on known identifiers/preprocessor symbols
        if (ImGui::IsMousePosValid())
        {
            std::string id = GetWordAt(ScreenPosToCoordinates(ImGui::GetMousePos()));
            if (!id.empty())
            {
                /*
                    auto it = m_LanguageDefinition.mIdentifiers.find(id);
                    if (it != m_LanguageDefinition.mIdentifiers.end())
                    {
                        ImGui::BeginTooltip();
                        ImGui::TextUnformatted(it->second.m_Declaration.c_str());
                        ImGui::EndTooltip();
                    }
                    else
                    {
                        auto pi = m_LanguageDefinition.mPreprocIdentifiers.find(id);
                        if (pi != m_LanguageDefinition.mPreprocIdentifiers.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::TextUnformatted(pi->second.m_Declaration.c_str());
                            ImGui::EndTooltip();
                        }
                    }
                    */
            }
        }

        ImGui::Dummy(ImVec2((longest + 2), m_Lines.size() * m_CharAdvance.y));

        if (m_ScrollToCursor)
        {
            EnsureCursorVisible();
            ImGui::SetWindowFocus();
            m_ScrollToCursor = false;
        }
    }

    void CodeEditor::ColorizeToken(const dlx::Token& token) noexcept
    {
        PaletteIndex palette_index{PaletteIndex::Default};

        // Determine palette color
        switch (token.GetType())
        {
            case dlx::Token::Type::Comment:
                palette_index = PaletteIndex::Comment;
                break;
            case dlx::Token::Type::ImmediateInteger:
            case dlx::Token::Type::IntegerLiteral:
                palette_index = PaletteIndex::IntegerLiteral;
                break;
            case dlx::Token::Type::OpCode:
                palette_index = PaletteIndex::OpCode;
                break;
            case dlx::Token::Type::RegisterFloat:
            case dlx::Token::Type::RegisterInt:
            case dlx::Token::Type::RegisterStatus:
                palette_index = PaletteIndex::Register;
                break;
            case dlx::Token::Type::NewLine:
                return;
            default:
                break;
        }

        Line& line = m_Lines[(token.GetLineNumber() - 1u).unsafe()];

        for (phi::u64 index{token.GetColumn() - 1u};
             index < token.GetColumn() + token.GetLength() - 1u; ++index)
        {
            PHI_DBG_ASSERT(index < line.size());
            line[index.unsafe()].m_ColorIndex = palette_index;
        }
    }

    void CodeEditor::ColorizeInternal() noexcept
    {
        const dlx::ParsedProgram& program = m_Emulator->GetProgram();

        for (const dlx::Token& token : program.m_Tokens)
        {
            ColorizeToken(token);
        }
    }

    void CodeEditor::ResetState() noexcept
    {
        m_State.m_CursorPosition = Coordinates(0, 0);
        m_State.m_SelectionStart = Coordinates(0, 0);
        m_State.m_SelectionEnd   = Coordinates(0, 0);
    }

    void CodeEditor::FixSelectionAfterMove(phi::boolean select, Coordinates old_pos) noexcept
    {
        if (select)
        {
            if (old_pos == m_InteractiveStart)
            {
                m_InteractiveStart = m_State.m_CursorPosition;
            }
            else if (old_pos == m_InteractiveEnd)
            {
                m_InteractiveEnd = m_State.m_CursorPosition;
            }
            else
            {
                m_InteractiveStart = m_State.m_CursorPosition;
                m_InteractiveEnd   = old_pos;
            }
        }
        else
        {
            m_InteractiveStart = m_InteractiveEnd = m_State.m_CursorPosition;
        }

        SetSelection(m_InteractiveStart, m_InteractiveEnd);
        EnsureCursorVisible();
    }

    phi::u8_fast CodeEditor::GetTabSizeAt(phi::i32 column) const noexcept
    {
        return static_cast<phi::uint_fast8_t>(m_TabSize.unsafe() -
                                              (column.unsafe() % m_TabSize.unsafe()));
    }

    ImU32 CodeEditor::GetPaletteForIndex(PaletteIndex index) const noexcept
    {
        phi::size_t int_value = static_cast<phi::size_t>(index);

        PHI_DBG_ASSERT(index != PaletteIndex::Max);
        PHI_DBG_ASSERT(int_value < m_Palette.size());

        return m_Palette[int_value];
    }
} // namespace dlxemu
