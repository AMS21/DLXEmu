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
#include <phi/algorithm/clamp.hpp>
#include <phi/algorithm/max.hpp>
#include <phi/algorithm/string_length.hpp>
#include <phi/algorithm/swap.hpp>
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
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
#include <phi/type_traits/to_underlying.hpp>

PHI_MSVC_SUPPRESS_WARNING_PUSH()
PHI_MSVC_SUPPRESS_WARNING(5262)

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h> // for imGui::GetCurrentWindow()
#include <imgui_internal.h>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_MSVC_SUPPRESS_WARNING_POP()
PHI_GCC_SUPPRESS_WARNING_POP()

PHI_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")
// TODO: Fix all the warnigns from gcc
PHI_GCC_SUPPRESS_WARNING("-Wsign-conversion")
PHI_GCC_SUPPRESS_WARNING("-Wstrict-overflow")
PHI_GCC_SUPPRESS_WARNING("-Wfloat-equal")

//#define DLXEMU_VERIFY_UNDO_REDO
//#define DLXEMU_VERIFY_COLUMN

// Free Helper functions

[[nodiscard]] static constexpr phi::boolean IsUTFSequence(const char character) noexcept
{
    return (character & 0xC0) == 0x80;
}

[[nodiscard]] static constexpr phi::boolean IsUTFSequence(const unsigned char character) noexcept
{
    return IsUTFSequence(static_cast<char>(character));
}

using namespace phi::literals;

// https://en.wikipedia.org/wiki/UTF-8
// We assume that the char is a standalone character (<128) or a leading byte of an UTF-8 code sequence (non-10xxxxxx code)
[[nodiscard]] static constexpr phi::u8_fast UTF8CharLength(const char characater) noexcept
{
    if ((characater & 0xFE) == 0xFC)
    {
        return 6_u8;
    }

    if ((characater & 0xFC) == 0xF8)
    {
        return 5_u8;
    }

    if ((characater & 0xF8) == 0xF0)
    {
        return 4_u8;
    }

    if ((characater & 0xF0) == 0xE0)
    {
        return 3_u8;
    }

    if ((characater & 0xE0) == 0xC0)
    {
        return 2_u8;
    }

    return 1_u8;
}

[[nodiscard]] static constexpr phi::u8_fast UTF8CharLength(const unsigned char characater) noexcept
{
    return UTF8CharLength(static_cast<char>(characater));
}

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
    //else if (character < 0x10000)
    {
        PHI_ASSERT(character.unsafe() < 0x10000);

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
        : m_Line(0u)
        , m_Column(0u)
    {}

    CodeEditor::Coordinates::Coordinates(phi::u32 line, phi::u32 column) noexcept
        : m_Line(line)
        , m_Column(column)
    {}

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::Coordinates::operator==(
            const Coordinates& other) const noexcept
    {
        return (m_Line == other.m_Line) && (m_Column == other.m_Column);
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::Coordinates::operator!=(
            const Coordinates& other) const noexcept
    {
        return (m_Line != other.m_Line) || (m_Column != other.m_Column);
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::Coordinates::operator<(
            const Coordinates& other) const noexcept
    {
        if (m_Line != other.m_Line)
        {
            return m_Line < other.m_Line;
        }

        return m_Column < other.m_Column;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::Coordinates::operator>(
            const Coordinates& other) const noexcept
    {
        if (m_Line != other.m_Line)
        {
            return m_Line > other.m_Line;
        }

        return m_Column > other.m_Column;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::Coordinates::operator<=(
            const Coordinates& other) const noexcept
    {
        if (m_Line != other.m_Line)
        {
            return m_Line < other.m_Line;
        }

        return m_Column <= other.m_Column;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::Coordinates::operator>=(
            const Coordinates& other) const noexcept
    {
        if (m_Line != other.m_Line)
        {
            return m_Line > other.m_Line;
        }

        return m_Column >= other.m_Column;
    }

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
        , m_ScrollToCursor(false)
        , m_ScrollToTop(false)
        , m_TextChanged(false)
        , m_ColorizerEnabled(true)
        , m_CursorPositionChanged(false)
        , m_TextStart(20.0f)
        , m_ColorRangeMin(0u)
        , m_ColorRangeMax(0u)
        , m_SelectionMode(SelectionMode::Normal)
        , m_ShowWhitespaces(false)
        , m_PaletteBase(GetDarkPalette())
        , m_Palette()
        , m_StartTime(static_cast<std::uint64_t>(
                  std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count()))
        , m_LastClick(-1.0f)
        , m_Emulator(emulator)
    {
        Colorize();
        m_Lines.push_back(Line());
    }

    CodeEditor::~CodeEditor() noexcept
    {
        VerifyInternalState();
    }

    PHI_ATTRIBUTE_CONST const CodeEditor::Palette& CodeEditor::GetPalette() const noexcept
    {
        return m_PaletteBase;
    }

    void CodeEditor::SetPalette(const Palette& value) noexcept
    {
        m_PaletteBase = value;

        UpdatePalette();
    }

    void CodeEditor::SetErrorMarkers(const ErrorMarkers& markers) noexcept
    {
        // Reject if any marker are invalid
        for (auto&& marker : markers)
        {
            const phi::u32 line_number = marker.first;

            if (line_number == 0u || line_number > m_Lines.size())
            {
                return;
            }
        }

        m_ErrorMarkers = markers;
    }

    void CodeEditor::AddErrorMarker(const phi::u32 line_number, const std::string& message) noexcept
    {
        if (line_number > m_Lines.size() || line_number == 0u)
        {
            return;
        }

        if (m_ErrorMarkers.contains(line_number.unsafe()))
        {
            m_ErrorMarkers[line_number.unsafe()] += '\n' + message;
        }
        else
        {
            m_ErrorMarkers[line_number.unsafe()] = message;
        }
    }

    void CodeEditor::ClearErrorMarkers() noexcept
    {
        m_ErrorMarkers.clear();
    }

    PHI_ATTRIBUTE_CONST CodeEditor::ErrorMarkers& CodeEditor::GetErrorMarkers() noexcept
    {
        return m_ErrorMarkers;
    }

    PHI_ATTRIBUTE_CONST const CodeEditor::ErrorMarkers& CodeEditor::GetErrorMarkers() const noexcept
    {
        return m_ErrorMarkers;
    }

    void CodeEditor::SetBreakpoints(const Breakpoints& markers) noexcept
    {
        // Reject aif any lines are invalid
        for (phi::u32 line_number : markers)
        {
            if (line_number == 0u || line_number > m_Lines.size())
            {
                return;
            }
        }

        m_Breakpoints = markers;
    }

    phi::boolean CodeEditor::AddBreakpoint(const phi::u32 line_number) noexcept
    {
        if (line_number > m_Lines.size() || line_number == 0u)
        {
            return false;
        }

        return m_Breakpoints.insert(line_number.unsafe()).second;
    }

    phi::boolean CodeEditor::RemoveBreakpoint(const phi::u32 line_number) noexcept
    {
        auto iterator = m_Breakpoints.find(line_number.unsafe());

        if (iterator != m_Breakpoints.end())
        {
            m_Breakpoints.erase(iterator);
            return true;
        }

        return false;
    }

    phi::boolean CodeEditor::ToggleBreakpoint(const phi::u32 line_number) noexcept
    {
        if (auto iterator = m_Breakpoints.find(line_number.unsafe());
            iterator != m_Breakpoints.end())
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

    PHI_ATTRIBUTE_CONST CodeEditor::Breakpoints& CodeEditor::GetBreakpoints() noexcept
    {
        return m_Breakpoints;
    }

    PHI_ATTRIBUTE_CONST const CodeEditor::Breakpoints& CodeEditor::GetBreakpoints() const noexcept
    {
        return m_Breakpoints;
    }

    void CodeEditor::Render(const ImVec2& size, phi::boolean border) noexcept
    {
        // Verify that ImGui is correctly initialzied
        PHI_ASSERT(GImGui && GImGui->Initialized, "ImGui was not initialized!");

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
            ImGui::BeginChild("Code Editor", sanitized_size, border.unsafe(),
                              ImGuiWindowFlags_HorizontalScrollbar |
                                      ImGuiWindowFlags_AlwaysHorizontalScrollbar |
                                      ImGuiWindowFlags_NoMove);

            // Need to calculate char advance before any inputs which use the values
            ComputeCharAdvance();

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
                    AddErrorMarker(static_cast<phi::uint32_t>(err.GetLineNumber()),
                                   err.ConstructMessage());
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
                m_Lines.back().emplace_back(
                        Glyph(static_cast<Char>(character), PaletteIndex::Default));
            }
        }

        m_TextChanged = true;
        m_ScrollToTop = true;

        m_UndoBuffer.clear();
        m_UndoIndex = 0u;

        Colorize();
    }

    PHI_ATTRIBUTE_PURE std::string CodeEditor::GetText() const noexcept
    {
        const phi::u32 max_line = GetMaxLineNumber();

        return GetText(Coordinates(0u, 0u), Coordinates(max_line, GetLineMaxColumn(max_line)));
    }

    void CodeEditor::ClearText() noexcept
    {
        PHI_ASSERT(!m_Lines.empty());

        if (IsReadOnly())
        {
            return;
        }

        // No text to clear
        if (m_Lines.size() == 1u && m_Lines[0u].empty())
        {
            return;
        }

        UndoRecord undo;
        undo.StoreBeforeState(this);

        undo.m_Removed      = GetText();
        undo.m_RemovedStart = Coordinates(0u, 0u);
        phi::u32 max_line   = GetMaxLineNumber();
        undo.m_RemovedEnd   = Coordinates(max_line, GetLineMaxColumn(max_line));

        m_Lines.clear();
        m_Lines.emplace_back(Line{});
        ResetState();

        undo.StoreAfterState(this);
        AddUndo(undo);
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
                                Glyph(static_cast<Char>(character), PaletteIndex::Default));
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
                text[i] = static_cast<char>(line[i].m_Char);
            }

            result.emplace_back(phi::move(text));
        }

        return result;
    }

    PHI_ATTRIBUTE_PURE std::string CodeEditor::GetSelectedText() const noexcept
    {
        return GetText(m_State.m_SelectionStart, m_State.m_SelectionEnd);
    }

    PHI_ATTRIBUTE_PURE std::string CodeEditor::GetCurrentLineText() const noexcept
    {
        const phi::u32 line_length = GetLineMaxColumn(m_State.m_CursorPosition.m_Line);

        return GetText(Coordinates(m_State.m_CursorPosition.m_Line, 0u),
                       Coordinates(m_State.m_CursorPosition.m_Line, line_length));
    }

    PHI_ATTRIBUTE_PURE phi::usize CodeEditor::GetTotalLines() const noexcept
    {
        return m_Lines.size();
    }

    void CodeEditor::SetOverwrite(phi::boolean overwrite) noexcept
    {
        m_Overwrite = overwrite;
    }

    void CodeEditor::ToggleOverwrite() noexcept
    {
        m_Overwrite = !m_Overwrite;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::IsOverwrite() const noexcept
    {
        return m_Overwrite;
    }

    void CodeEditor::SetReadOnly(phi::boolean value) noexcept
    {
        m_ReadOnly = value;
    }

    void CodeEditor::ToggleReadOnly() noexcept
    {
        m_ReadOnly = !m_ReadOnly;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::IsReadOnly() const noexcept
    {
        return m_ReadOnly;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::IsTextChanged() const noexcept
    {
        return m_TextChanged;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::IsCursorPositionChanged() const noexcept
    {
        return m_CursorPositionChanged;
    }

    void CodeEditor::SetColorizerEnable(phi::boolean value) noexcept
    {
        m_ColorizerEnabled = value;
    }

    void CodeEditor::ToggleColorizerEnabled() noexcept
    {
        m_ColorizerEnabled = !m_ColorizerEnabled;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::IsColorizerEnabled() const noexcept
    {
        return m_ColorizerEnabled;
    }

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::GetCursorPosition() const noexcept
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

    void CodeEditor::SetShowWhitespaces(phi::boolean value) noexcept
    {
        m_ShowWhitespaces = value;
    }

    void CodeEditor::ToggleShowWhitespaces() noexcept
    {
        m_ShowWhitespaces = !m_ShowWhitespaces;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::IsShowingWhitespaces() const noexcept
    {
        return m_ShowWhitespaces;
    }

    void CodeEditor::SetTabSize(phi::u8_fast new_tab_size) noexcept
    {
        new_tab_size = phi::clamp(new_tab_size, MinTabSize, MaxTabSize);

        if (new_tab_size != m_TabSize)
        {
            // Save old character indexes
            const phi::u32 cursor_char_index          = GetCharacterIndex(m_State.m_CursorPosition);
            const phi::u32 selection_start_char_index = GetCharacterIndex(m_State.m_SelectionStart);
            const phi::u32 selection_end_char_index   = GetCharacterIndex(m_State.m_SelectionEnd);

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

    PHI_ATTRIBUTE_PURE phi::u8_fast CodeEditor::GetTabSize() const noexcept
    {
        return m_TabSize;
    }

    void CodeEditor::EnterCharacter(ImWchar character, phi::boolean shift) noexcept
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
        Coordinates start       = phi::min(pos, m_State.m_SelectionStart);
        phi::u32    total_lines = pos.m_Line - start.m_Line;

        total_lines += InsertTextAt(pos, value);

        SetSelection(pos, pos);
        SetCursorPosition(pos);

        Colorize(start.m_Line, total_lines);
    }

    void CodeEditor::MoveUp(phi::u32 amount, phi::boolean select) noexcept
    {
        if (amount == 0u)
        {
            return;
        }

        const Coordinates old_pos = m_State.m_CursorPosition;

        // Move cursor to the beginning of the first line
        if (amount > old_pos.m_Line)
        {
            m_State.m_CursorPosition.m_Line   = 0u;
            m_State.m_CursorPosition.m_Column = 0u;
        }
        else
        {
            PHI_ASSERT(amount <= m_State.m_CursorPosition.m_Line);
            m_State.m_CursorPosition.m_Line -= amount;
        }

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveDown(phi::u32 amount, phi::boolean select) noexcept
    {
        if (amount == 0u)
        {
            return;
        }

        const Coordinates old_pos  = m_State.m_CursorPosition;
        const phi::u32    max_line = GetMaxLineNumber();

        PHI_ASSERT(old_pos.m_Line <= max_line);

        // Move cursor to the end of the line after we reached the bottom
        if (amount > max_line - old_pos.m_Line)
        {
            m_State.m_CursorPosition.m_Line   = max_line;
            m_State.m_CursorPosition.m_Column = GetLineMaxColumn(max_line);
        }
        else
        {
            m_State.m_CursorPosition.m_Line += amount;
        }

        PHI_ASSERT(m_State.m_CursorPosition.m_Line <= max_line);

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveLeft(phi::u32 amount, phi::boolean select, phi::boolean word_mode) noexcept
    {
        PHI_ASSERT(!m_Lines.empty());

        if (amount == 0u)
        {
            return;
        }

        const Coordinates old_pos = m_State.m_CursorPosition;
        m_State.m_CursorPosition  = GetActualCursorCoordinates();
        phi::u32 line             = m_State.m_CursorPosition.m_Line;
        phi::u32 cindex           = GetCharacterIndex(m_State.m_CursorPosition);

        while (amount > 0u)
        {
            amount -= 1u;

            if (cindex == 0u)
            {
                if (line == 0u)
                {
                    // We're already on the last line so we can't move further left
                    amount = 0u;
                }
                else
                {
                    line -= 1u;
                    cindex = static_cast<phi::uint32_t>(m_Lines[line.unsafe()].size());
                }
            }
            else
            {
                --cindex;
                if (cindex > 0u && m_Lines.size() > line)
                {
                    while (cindex > 0u &&
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
        PHI_ASSERT(m_State.m_CursorPosition.m_Column >= 0u);

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveRight(phi::u32 amount, phi::boolean select,
                               phi::boolean word_mode) noexcept
    {
        const Coordinates old_pos = m_State.m_CursorPosition;

        if (old_pos.m_Line >= m_Lines.size() || amount == 0u)
        {
            return;
        }

        phi::u32 cindex = GetCharacterIndex(m_State.m_CursorPosition);
        while (amount > 0u)
        {
            amount -= 1u;

            const phi::u32 lindex = m_State.m_CursorPosition.m_Line;
            PHI_ASSERT(lindex < m_Lines.size());
            const Line& line = m_Lines[lindex.unsafe()];

            if (cindex >= line.size())
            {
                if (m_State.m_CursorPosition.m_Line < GetMaxLineNumber())
                {
                    m_State.m_CursorPosition.m_Line =
                            phi::clamp(m_State.m_CursorPosition.m_Line.unsafe() + 1u, 0u,
                                       GetMaxLineNumber().unsafe());
                    m_State.m_CursorPosition.m_Column = 0u;
                }
                else
                {
                    return;
                }
            }
            else
            {
                PHI_ASSERT(cindex < line.size());
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

    void CodeEditor::MoveTop(phi::boolean select) noexcept
    {
        const Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(Coordinates(0u, 0u));

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

    void CodeEditor::CodeEditor::MoveBottom(phi::boolean select) noexcept
    {
        const Coordinates old_pos = GetCursorPosition();

        const phi::u32 end_line = GetMaxLineNumber();
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

    void CodeEditor::MoveHome(phi::boolean select) noexcept
    {
        const Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(Coordinates(m_State.m_CursorPosition.m_Line, 0u));

        FixSelectionAfterMove(select, old_pos);
    }

    void CodeEditor::MoveEnd(phi::boolean select) noexcept
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

    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

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
                const phi::u32 line_no   = m_State.m_SelectionEnd.m_Line;
                m_State.m_SelectionStart = Coordinates(m_State.m_SelectionStart.m_Line, 0u);
                m_State.m_SelectionEnd   = Coordinates(line_no, GetLineMaxColumn(line_no));
                break;
            }
#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_ASSERT_NOT_REACHED();
#endif
        }

        if (m_State.m_SelectionStart != old_sel_start || m_State.m_SelectionEnd != old_sel_end)
        {
            m_CursorPositionChanged = true;
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()

    void CodeEditor::SelectWordUnderCursor() noexcept
    {
        Coordinates coords = GetCursorPosition();
        SetSelection(FindWordStart(coords), FindWordEnd(coords));
    }

    void CodeEditor::SelectAll() noexcept
    {
        SetSelection(Coordinates(0u, 0u),
                     Coordinates(static_cast<phi::uint32_t>(m_Lines.size()), 0u));
    }

    void CodeEditor::ClearSelection() noexcept
    {
        SetSelection(Coordinates(0u, 0u), Coordinates(0u, 0u));
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::HasSelection() const noexcept
    {
        return m_State.m_SelectionEnd > m_State.m_SelectionStart;
    }

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::GetSelectionStart() const noexcept
    {
        return m_State.m_SelectionStart;
    }

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::GetSelectionEnd() const noexcept
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
            PHI_ASSERT(!m_Lines.empty());

            std::string str;
            const Line& line = m_Lines[GetActualCursorCoordinates().m_Line.unsafe()];

            for (const Glyph& glyph : line)
            {
                str.push_back(static_cast<char>(glyph.m_Char));
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
                UndoRecord undo;
                undo.StoreBeforeState(this);
                undo.m_Removed      = GetSelectedText();
                undo.m_RemovedStart = m_State.m_SelectionStart;
                undo.m_RemovedEnd   = m_State.m_SelectionEnd;

                Copy();
                DeleteSelection();

                undo.StoreAfterState(this);
                AddUndo(undo);
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
        PHI_ASSERT(clip_text);
        if (phi::string_length(clip_text) == 0u)
        {
            return;
        }

        UndoRecord undo;
        undo.StoreBeforeState(this);

        if (HasSelection())
        {
            undo.m_Removed      = GetSelectedText();
            undo.m_RemovedStart = m_State.m_SelectionStart;
            undo.m_RemovedEnd   = m_State.m_SelectionEnd;
            DeleteSelection();
        }

        undo.m_Added      = clip_text;
        undo.m_AddedStart = GetActualCursorCoordinates();

        InsertText(clip_text);

        undo.m_AddedEnd = GetActualCursorCoordinates();
        undo.m_After    = m_State;
        AddUndo(undo);
    }

    void CodeEditor::Delete() noexcept
    {
        PHI_ASSERT(!m_Lines.empty());

        if (m_ReadOnly)
        {
            return;
        }

        UndoRecord undo;
        undo.StoreBeforeState(this);

        if (HasSelection())
        {
            undo.m_Removed      = GetSelectedText();
            undo.m_RemovedStart = m_State.m_SelectionStart;
            undo.m_RemovedEnd   = m_State.m_SelectionEnd;

            DeleteSelection();
        }
        else
        {
            Coordinates pos = GetActualCursorCoordinates();
            SetCursorPosition(pos);
            PHI_ASSERT(pos.m_Line < m_Lines.size());
            Line& line = m_Lines[pos.m_Line.unsafe()];

            if (pos.m_Column == GetLineMaxColumn(pos.m_Line))
            {
                if (pos.m_Line == GetMaxLineNumber())
                {
                    return;
                }

                undo.m_Removed      = '\n';
                undo.m_RemovedStart = undo.m_RemovedEnd = GetActualCursorCoordinates();
                Advance(undo.m_RemovedEnd);

                PHI_ASSERT(pos.m_Line + 1u < m_Lines.size());
                Line& next_line = m_Lines[pos.m_Line.unsafe() + 1];
                line.insert(line.end(), next_line.begin(), next_line.end());

                PHI_ASSERT(pos.m_Line <= m_Lines.size());
                RemoveLine(pos.m_Line + 1u);
            }
            else
            {
                // Nothing todo when line is empty
                if (line.empty())
                {
                    return;
                }

                const phi::u32 cindex = GetCharacterIndex(pos);
                PHI_ASSERT(cindex < line.size());

                const Coordinates current_cursor_pos = GetActualCursorCoordinates();
                undo.m_RemovedStart                  = current_cursor_pos;
                undo.m_RemovedEnd                    = current_cursor_pos;
                undo.m_RemovedEnd.m_Column++;
                undo.m_Removed = GetText(undo.m_RemovedStart, undo.m_RemovedEnd);

                phi::u8_fast length = UTF8CharLength(line[cindex.unsafe()].m_Char);
                while (length > 0u && cindex < line.size())
                {
                    line.erase(line.begin() + cindex.unsafe());

                    // Correct selection state
                    if (m_State.m_SelectionStart.m_Line == current_cursor_pos.m_Line &&
                        m_State.m_SelectionStart.m_Column >= cindex &&
                        m_State.m_SelectionStart.m_Column > 0u)
                    {
                        m_State.m_SelectionStart.m_Column -= 1u;
                    }
                    if (m_State.m_SelectionEnd.m_Line == current_cursor_pos.m_Line &&
                        m_State.m_SelectionEnd.m_Column >= cindex &&
                        m_State.m_SelectionEnd.m_Column > 0u)
                    {
                        m_State.m_SelectionEnd.m_Column -= 1u;
                    }

                    length--;
                }
            }

            m_TextChanged = true;

            Colorize(pos.m_Line, 1);
        }

        undo.StoreAfterState(this);
        AddUndo(undo);
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::CanUndo() const noexcept
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

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::CanRedo() const noexcept
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
        str += fmt::format("Actual cursor position: {:d}, {:d}\n", cursor_pos.m_Line.unsafe(),
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
        str += fmt::format("Selection mode: {:s}\n", dlx::enum_name(m_SelectionMode));

        const std::string              full_text = GetText();
        const std::vector<std::string> lines     = GetTextLines();
        std::string                    lines_text;

        for (const std::string& line : lines)
        {
            lines_text += line + '\n';
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
            str += fmt::format("{:02d}: {:s}\n", marker.first, marker.second);
        }

        str += "\n";
        str += "Break points:\n";
        if (GetBreakpoints().empty())
        {
            str += "None\n";
        }
        for (const auto break_point : GetBreakpoints())
        {
            str += fmt::format("{:02d}\n", break_point);
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

    void CodeEditor::UpdatePalette() noexcept
    {
        PHI_ASSERT(GImGui && GImGui->Initialized);

        // Update palette with the current alpha from style
        for (phi::usize i = 0u; i < phi::to_underlying(PaletteIndex::Max); ++i)
        {
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(m_PaletteBase[i]);
            color.w *= ImGui::GetStyle().Alpha;
            m_Palette[i] = ImGui::ColorConvertFloat4ToU32(color);
        }
    }

    void CodeEditor::VerifyInternalState() const noexcept
    {
        // Lines should never be empty
        PHI_ASSERT(!m_Lines.empty());

        // Verify Selection is still in a valid state
        PHI_ASSERT(m_State.m_SelectionEnd >= m_State.m_SelectionStart);
        PHI_ASSERT(m_State.m_SelectionStart.m_Line < m_Lines.size());
        PHI_ASSERT(m_State.m_SelectionStart.m_Column >= 0u);
        PHI_ASSERT(m_State.m_SelectionEnd.m_Line < m_Lines.size());
        PHI_ASSERT(m_State.m_SelectionEnd.m_Column >= 0u);

        // Verify cursor position
        PHI_ASSERT(m_State.m_CursorPosition.m_Line >= 0u);
        PHI_ASSERT(m_State.m_CursorPosition.m_Line < m_Lines.size());
        PHI_ASSERT(m_State.m_CursorPosition.m_Column >= 0u);

        // This should also always be true. But its implementation is way to slow when fuzzing
#if defined(DLXEMU_VERIFY_COLUMN)
        PHI_ASSERT(m_State.m_SelectionStart.m_Column <=
                   GetLineMaxColumn(m_State.m_SelectionStart.m_Line));
        PHI_ASSERT(m_State.m_SelectionEnd.m_Column <=
                   GetLineMaxColumn(m_State.m_SelectionEnd.m_Line));
#endif

        // Verify that all breakpoints are on existing lines
        for (phi::u32 break_point : m_Breakpoints)
        {
            PHI_ASSERT(break_point != 0u);
            PHI_ASSERT(break_point <= m_Lines.size());
        }

        // Verify that all error markers are on existing lines
        for (auto&& error_marker : m_ErrorMarkers)
        {
            const phi::u32 line_number = error_marker.first;

            PHI_ASSERT(line_number != 0u);
            PHI_ASSERT(line_number <= m_Lines.size());
        }
    }

    PHI_ATTRIBUTE_CONST const CodeEditor::Palette& CodeEditor::GetDarkPalette() noexcept
    {
        constexpr const static Palette palette = {{
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

        return palette;
    }

    PHI_ATTRIBUTE_CONST const CodeEditor::Palette& CodeEditor::GetLightPalette() noexcept
    {
        constexpr const static Palette palette = {{
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

        return palette;
    }

    PHI_ATTRIBUTE_CONST const CodeEditor::Palette& CodeEditor::GetRetroBluePalette() noexcept
    {
        constexpr const static Palette palette = {{
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

        return palette;
    }

    // EditorState
    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::EditorState::operator==(
            const EditorState& other) const noexcept
    {
        return other.m_CursorPosition == m_CursorPosition &&
               other.m_SelectionStart == m_SelectionStart && other.m_SelectionEnd == m_SelectionEnd;
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::EditorState::operator!=(
            const EditorState& other) const noexcept
    {
        return other.m_CursorPosition != m_CursorPosition ||
               other.m_SelectionStart != m_SelectionStart || other.m_SelectionEnd != m_SelectionEnd;
    }

    // UndoRecord
    void CodeEditor::UndoRecord::Undo(CodeEditor* editor) const noexcept
    {
        PHI_ASSERT(editor != nullptr);

        if (!m_Added.empty())
        {
            editor->DeleteRange(m_AddedStart, m_AddedEnd);
            editor->Colorize(m_AddedStart.m_Line, m_AddedEnd.m_Line - m_AddedStart.m_Line);
        }

        if (!m_Removed.empty())
        {
            Coordinates start = m_RemovedStart;
            editor->InsertTextAt(start, m_Removed.c_str());
            editor->Colorize(start.m_Line, m_RemovedEnd.m_Line - m_RemovedStart.m_Line);
        }

        ApplyBeforeState(editor);
        editor->EnsureCursorVisible();
    }

    void CodeEditor::UndoRecord::Redo(CodeEditor* editor) const noexcept
    {
        PHI_ASSERT(editor != nullptr);

        if (!m_Removed.empty())
        {
            editor->DeleteRange(m_RemovedStart, m_RemovedEnd);
            editor->Colorize(m_RemovedStart.m_Line, m_RemovedEnd.m_Line - m_RemovedStart.m_Line);
        }

        if (!m_Added.empty())
        {
            Coordinates start = m_AddedStart;
            editor->InsertTextAt(start, m_Added.c_str());
            editor->Colorize(start.m_Line, m_AddedEnd.m_Line - m_AddedStart.m_Line);
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

    void CodeEditor::Colorize(phi::u32 from_line, phi::i64 count) noexcept
    {
        // TODO: This is quite unreadable
        const phi::u32 to_line =
                count == -1 ? static_cast<phi::uint32_t>(m_Lines.size()) :
                              phi::min(static_cast<phi::uint32_t>(m_Lines.size()),
                                       from_line + static_cast<phi::uint32_t>(count.unsafe()));

        m_ColorRangeMin = phi::min(m_ColorRangeMin, from_line);
        m_ColorRangeMax = phi::max(m_ColorRangeMax, to_line);
        m_ColorRangeMin = phi::max(0u, m_ColorRangeMin);
        m_ColorRangeMax = phi::max(m_ColorRangeMin, m_ColorRangeMax);
    }

    float CodeEditor::TextDistanceToLineStart(const Coordinates& from) const noexcept
    {
        PHI_ASSERT(from.m_Line < m_Lines.size());

        const Line& line     = m_Lines[from.m_Line.unsafe()];
        float       distance = 0.0f;
        float       space_size =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr)
                        .x;
        const phi::u32 col_index = GetCharacterIndex(from);
        for (phi::usize it{0u}; it < line.size() && it < col_index;)
        {
            if (line[it.unsafe()].m_Char == '\t')
            {
                distance = (1.0f + std::floor((1.0f + distance) /
                                              (float(m_TabSize.unsafe()) * space_size))) *
                           (float(m_TabSize.unsafe()) * space_size);
                ++it;
            }
            else
            {
                phi::u8_fast        length = UTF8CharLength(line[it.unsafe()].m_Char);
                phi::array<char, 7> temp_c_string;
                phi::usize          index = 0u;
                for (; index < 6u && length > 0u && it < line.size(); ++index, ++it, --length)
                {
                    temp_c_string[index] = static_cast<char>(line[it.unsafe()].m_Char);
                }

                temp_c_string[index] = '\0';
                distance += ImGui::GetFont()
                                    ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f,
                                                    temp_c_string.data(), nullptr, nullptr)
                                    .x;
            }
        }

        return distance;
    }

    void CodeEditor::EnsureCursorVisible() noexcept
    {
        m_ScrollToCursor = true;
    }

    void CodeEditor::ScrollToCursor() noexcept
    {
        PHI_ASSERT(m_ScrollToCursor);

        float scroll_x = ImGui::GetScrollX();
        float scroll_y = ImGui::GetScrollY();

        float height = ImGui::GetWindowHeight();
        float width  = ImGui::GetWindowWidth();

        PHI_ASSERT(m_CharAdvance.x != 0.0f);
        PHI_ASSERT(m_CharAdvance.y != 0.0f);
        phi::u32 top = 1u + static_cast<phi::uint32_t>(std::ceil(scroll_y / m_CharAdvance.y));
        phi::u32 bottom =
                static_cast<phi::uint32_t>(std::ceil((scroll_y + height) / m_CharAdvance.y));

        phi::u32 left = static_cast<phi::uint32_t>(std::ceil(scroll_x / m_CharAdvance.x));
        phi::u32 right =
                static_cast<phi::uint32_t>(std::ceil((scroll_x + width) / m_CharAdvance.x));

        Coordinates pos = GetActualCursorCoordinates();
        float       len = TextDistanceToLineStart(pos);

        // TODO: The 4 here seems like it could very well be a constant

        if (pos.m_Line < top)
        {
            ImGui::SetScrollY(phi::max(0.0f, static_cast<float>(pos.m_Line.unsafe()) - 1.0f) *
                              m_CharAdvance.y);
        }
        if (pos.m_Line > bottom.unsafe() - 4u)
        {
            ImGui::SetScrollY(phi::max(
                    0.0f,
                    static_cast<float>((pos.m_Line + 4u).unsafe()) * m_CharAdvance.y - height));
        }
        if (len + m_TextStart < static_cast<float>((left + 4u).unsafe()))
        {
            ImGui::SetScrollX(phi::max(0.0f, len + m_TextStart - 4.0f));
        }
        if (len + m_TextStart > static_cast<float>(right.unsafe()) - 4.0f)
        {
            ImGui::SetScrollX(phi::max(0.0f, len + m_TextStart + 4.0f - width));
        }
    }

    PHI_ATTRIBUTE_PURE phi::u32 CodeEditor::GetPageSize() const noexcept
    {
        const float height = ImGui::GetWindowHeight() - 20.0f;

        PHI_ASSERT(m_CharAdvance.y != 0.0f);
        const float page_size = std::floor(height / m_CharAdvance.y);

        return static_cast<phi::uint32_t>(phi::max(page_size, 5.0f));
    }

    PHI_ATTRIBUTE_PURE std::string CodeEditor::GetText(const Coordinates& start,
                                                       const Coordinates& end) const noexcept
    {
        PHI_ASSERT(start.m_Line < m_Lines.size());
        PHI_ASSERT(end.m_Line < m_Lines.size());
        PHI_ASSERT(start <= end);

        std::string result;

        phi::u32   lstart = start.m_Line;
        phi::u32   lend   = end.m_Line;
        phi::u32   istart = GetCharacterIndex(start);
        phi::u32   iend   = phi::min(GetCharacterIndex(end),
                                     static_cast<phi::uint32_t>(m_Lines[end.m_Line.unsafe()].size()));
        phi::usize size   = 0u;

        for (phi::u32 i = lstart; i <= lend; ++i)
        {
            PHI_ASSERT(i < m_Lines.size());
            size += m_Lines[i.unsafe()].size();
        }

        result.reserve((size + size / 8u).unsafe());

        while (istart < iend || lstart < lend)
        {
            PHI_ASSERT(lstart < m_Lines.size());

            const Line& line = m_Lines[lstart.unsafe()];
            if (istart < line.size())
            {
                result += static_cast<char>(line[istart.unsafe()].m_Char);
                istart++;
            }
            else
            {
                istart = 0u;
                ++lstart;

                if (lstart != m_Lines.size())
                {
                    result += '\n';
                }
            }
        }

        return result;
    }

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::GetActualCursorCoordinates()
            const noexcept
    {
        return SanitizeCoordinates(m_State.m_CursorPosition);
    }

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::SanitizeCoordinates(
            const Coordinates& value) const noexcept
    {
        phi::u32 line   = value.m_Line;
        phi::u32 column = value.m_Column;

        if (line >= m_Lines.size())
        {
            PHI_ASSERT(!m_Lines.empty());

            line   = GetMaxLineNumber();
            column = GetLineMaxColumn(line);

            return {line, column};
        }

        PHI_ASSERT(!m_Lines.empty());
        PHI_ASSERT(line < m_Lines.size());

        // Sanitize column
        const Line& current_line = m_Lines[line.unsafe()];
        phi::u32    new_column   = 0u;
        for (phi::u32 char_index{0u}; char_index < current_line.size();)
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
        PHI_ASSERT(new_column <= GetLineMaxColumn(line));

        column = new_column;

        return {line, column};
    }

    void CodeEditor::Advance(Coordinates& coordinates) const noexcept
    {
        if (coordinates.m_Line >= m_Lines.size())
        {
            return;
        }

        const Line& line   = m_Lines[coordinates.m_Line.unsafe()];
        phi::u32    cindex = GetCharacterIndex(coordinates);

        if (cindex + 1u < line.size())
        {
            PHI_ASSERT(cindex < line.size());
            phi::u8_fast delta = UTF8CharLength(line[cindex.unsafe()].m_Char);
            cindex = phi::min(cindex + delta, static_cast<phi::uint32_t>(line.size() - 1u));
        }
        else
        {
            ++coordinates.m_Line;
            cindex = 0u;
        }
        coordinates.m_Column = GetCharacterColumn(coordinates.m_Line, cindex);
    }

    void CodeEditor::DeleteRange(const Coordinates& start, const Coordinates& end) noexcept
    {
        PHI_ASSERT(end > start);
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(start.m_Line < m_Lines.size());
        PHI_ASSERT(end.m_Line < m_Lines.size());

        const phi::u32 start_index = GetCharacterIndex(start);
        const phi::u32 end_index   = GetCharacterIndex(end);

        if (start.m_Line == end.m_Line)
        {
            Line&          line       = m_Lines[start.m_Line.unsafe()];
            const phi::u32 max_column = GetLineMaxColumn(start.m_Line);

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
                const phi::u32 start_column = GetCharacterColumn(start.m_Line, start_index);
                const phi::u32 end_column   = GetCharacterColumn(end.m_Line, end_index);

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
                RemoveLine(start.m_Line + 1u, end.m_Line + 1u);
            }
        }

        m_TextChanged = true;
    }

    phi::u32 CodeEditor::InsertTextAt(Coordinates& /* inout */ where, const char* value) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(where.m_Line < m_Lines.size());

        phi::u32 cindex      = GetCharacterIndex(where);
        phi::u32 total_lines = 0u;
        while (*value != '\0')
        {
            PHI_ASSERT(!m_Lines.empty());

            if (*value == '\n')
            {
                if (cindex < m_Lines[where.m_Line.unsafe()].size())
                {
                    Line& new_line = InsertLine(where.m_Line + 1u);
                    Line& line     = m_Lines[where.m_Line.unsafe()];
                    new_line.insert(new_line.begin(), line.begin() + cindex.unsafe(), line.end());
                    line.erase(line.begin() + cindex.unsafe(), line.end());
                }
                else
                {
                    InsertLine(where.m_Line + 1u);
                }

                ++where.m_Line;
                where.m_Column = 0u;
                cindex         = 0u;
                ++total_lines;
                ++value;
            }
            else if (*value == '\t')
            {
                Line& line = m_Lines[where.m_Line.unsafe()];

                line.insert(line.begin() + cindex.unsafe(),
                            Glyph(static_cast<Char>(*value++), PaletteIndex::Default));
                cindex += 1u;

                where.m_Column += GetTabSizeAt(where.m_Column);
            }
            else
            {
                Line& line = m_Lines[where.m_Line.unsafe()];

                for (phi::u8_fast length = UTF8CharLength(*value); length > 0u && *value != '\0';
                     ++cindex, --length)
                {
                    line.insert(line.begin() + cindex.unsafe(),
                                Glyph(static_cast<Char>(*value++), PaletteIndex::Default));
                }

                ++where.m_Column;
            }

            m_TextChanged = true;
        }

        return total_lines;
    }

    void CodeEditor::AddUndo(UndoRecord& value) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);

#if defined(DLXEMU_VERIFY_UNDO_REDO)
        VerifyInternalState();
        // Reject empty undos
        PHI_ASSERT(!(value.m_Added.empty() && value.m_Removed.empty()));
        // Start and end are valid
        PHI_ASSERT(value.m_AddedStart <= value.m_AddedEnd);
        PHI_ASSERT(value.m_RemovedStart <= value.m_RemovedEnd);
#endif

        m_UndoBuffer.resize((m_UndoIndex + 1u).unsafe());
        m_UndoBuffer.back() = value;
        ++m_UndoIndex;

#if defined(DLXEMU_VERIFY_UNDO_REDO)
        VerifyInternalState();

        PHI_ASSERT(CanUndo());

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

        PHI_ASSERT(CanRedo());

        // Test the redo
        Redo();
        VerifyInternalState();

        const std::string text_after          = GetText();
        EditorState       state_after         = m_State;
        state_after.m_CursorPosition.m_Column = GetCharacterIndex(state_after.m_CursorPosition);
        state_after.m_SelectionStart.m_Column = GetCharacterIndex(state_after.m_SelectionStart);
        state_after.m_SelectionEnd.m_Column   = GetCharacterIndex(state_after.m_SelectionEnd);

        PHI_ASSERT(text_before == text_after);
        PHI_ASSERT(state_before == state_after);
#endif
    }

    CodeEditor::Coordinates CodeEditor::ScreenPosToCoordinates(
            const ImVec2& position) const noexcept
    {
        const ImVec2 origin = ImGui::GetCursorScreenPos();
        const ImVec2 local(position.x - origin.x, position.y - origin.y);

        PHI_ASSERT(m_CharAdvance.y != 0.0f);
        const phi::u32 line_no =
                phi::max(0u, static_cast<phi::int32_t>(std::floor(local.y / m_CharAdvance.y)));

        phi::u32 column_coord = 0u;

        if (line_no >= 0u && line_no < m_Lines.size())
        {
            const Line& line = m_Lines[line_no.unsafe()];

            phi::u32 column_index = 0u;
            float    column_x     = 0.0f;

            while (column_index < line.size())
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
                    column_index += 1u;
                }
                else
                {
                    phi::array<char, 7u> buffer;
                    phi::usize           index = 0u;

                    for (phi::u8_fast length = UTF8CharLength(line[column_index.unsafe()].m_Char);
                         index < 6u && length > 0u; ++index, ++column_index, --length)
                    {
                        buffer[index] = static_cast<char>(line[column_index.unsafe()].m_Char);
                    }

                    buffer[index] = '\0';
                    column_width  = ImGui::GetFont()
                                           ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f,
                                                           buffer.data())
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

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::FindWordStart(
            const Coordinates& from) const noexcept
    {
        if (from.m_Line >= m_Lines.size())
        {
            return from;
        }

        const Line& line   = m_Lines[from.m_Line.unsafe()];
        phi::u32    cindex = GetCharacterIndex(from);

        if (cindex >= line.size())
        {
            return from;
        }

        while (cindex > 0u && phi::is_space(static_cast<char>(line[cindex.unsafe()].m_Char)))
        {
            --cindex;
        }

        const PaletteIndex cstart = line[cindex.unsafe()].m_ColorIndex;
        while (cindex > 0u)
        {
            Glyph glyph = line[cindex.unsafe()];
            if (!IsUTFSequence(glyph.m_Char))
            {
                if (glyph.m_Char <= 32 && phi::is_space(static_cast<char>(glyph.m_Char)))
                {
                    cindex += 1u;
                    break;
                }

                if (cstart != line[phi::size_t(cindex.unsafe() - 1u)].m_ColorIndex)
                {
                    break;
                }
            }

            cindex -= 1u;
        }

        return {from.m_Line, GetCharacterColumn(from.m_Line, cindex)};
    }

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::FindWordEnd(
            const Coordinates& from) const noexcept
    {
        Coordinates at = from;
        if (at.m_Line >= m_Lines.size())
        {
            return at;
        }

        const Line& line   = m_Lines[at.m_Line.unsafe()];
        phi::u32    cindex = GetCharacterIndex(at);

        if (cindex >= line.size())
        {
            return at;
        }

        const phi::boolean prevspace =
                phi::is_space(static_cast<char>(line[cindex.unsafe()].m_Char));
        const PaletteIndex cstart = line[cindex.unsafe()].m_ColorIndex;
        while (cindex < line.size())
        {
            const Glyph glyph = line[cindex.unsafe()];

            if (cstart != line[cindex.unsafe()].m_ColorIndex)
            {
                break;
            }

            if (prevspace != phi::is_space(static_cast<char>(glyph.m_Char)))
            {
                if (phi::is_space(static_cast<char>(glyph.m_Char)))
                {
                    while (cindex < line.size() &&
                           phi::is_space(static_cast<char>(line[cindex.unsafe()].m_Char)))
                    {
                        cindex += 1u;
                    }
                }
                break;
            }

            cindex += UTF8CharLength(glyph.m_Char);
        }

        return {from.m_Line, GetCharacterColumn(from.m_Line, cindex)};
    }

    PHI_ATTRIBUTE_PURE CodeEditor::Coordinates CodeEditor::FindNextWord(
            const Coordinates& from) const noexcept
    {
        Coordinates at = from;
        if (at.m_Line >= m_Lines.size())
        {
            return at;
        }

        // skip to the next non-word character
        phi::u32     cindex  = GetCharacterIndex(from);
        phi::boolean is_word = false;
        phi::boolean skip    = false;
        if (cindex < m_Lines[at.m_Line.unsafe()].size())
        {
            const Line& line = m_Lines[at.m_Line.unsafe()];
            is_word = phi::is_alpha_numeric(static_cast<char>(line[cindex.unsafe()].m_Char));
            skip    = is_word;
        }

        while (!is_word || skip)
        {
            if (at.m_Line >= m_Lines.size())
            {
                const phi::u32 line_number = GetMaxLineNumber();

                return {line_number, GetLineMaxColumn(line_number)};
            }

            const Line& line = m_Lines[at.m_Line.unsafe()];
            if (cindex < line.size())
            {
                is_word = phi::is_alpha_numeric(static_cast<char>(line[cindex.unsafe()].m_Char));

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
                cindex = 0u;
                at.m_Line += 1u;
                skip    = false;
                is_word = false;
            }
        }

        return at;
    }

    PHI_ATTRIBUTE_PURE std::string CodeEditor::GetWordUnderCursor() const noexcept
    {
        const Coordinates coordinates = GetCursorPosition();
        return GetWordAt(coordinates);
    }

    PHI_ATTRIBUTE_PURE std::string CodeEditor::GetWordAt(const Coordinates& coords) const noexcept
    {
        PHI_ASSERT(coords.m_Line < m_Lines.size());

        Coordinates start = FindWordStart(coords);
        Coordinates end   = FindWordEnd(coords);

        phi::u32 istart = GetCharacterIndex(start);
        phi::u32 iend   = GetCharacterIndex(end);

        std::string result;

        for (phi::u32 iterator = istart; iterator < iend; ++iterator)
        {
            const Line& line = m_Lines[coords.m_Line.unsafe()];

            PHI_ASSERT(iterator < line.size());
            result.push_back(static_cast<char>(line[iterator.unsafe()].m_Char));
        }

        return result;
    }

    PHI_ATTRIBUTE_PURE phi::u32 CodeEditor::GetCharacterIndex(
            const Coordinates& coordinates) const noexcept
    {
        PHI_ASSERT(coordinates.m_Line < m_Lines.size());

        const Line& line  = m_Lines[coordinates.m_Line.unsafe()];
        phi::u32    index = 0u;
        for (phi::u32 column{0u}; index < line.size() && column < coordinates.m_Column;)
        {
            PHI_ASSERT(index < line.size());
            const Char character = line[index.unsafe()].m_Char;

            if (character == '\t')
            {
                column = (column / m_TabSize) * m_TabSize + m_TabSize;
            }
            else
            {
                ++column;
            }

            index += UTF8CharLength(character);
        }

        return index;
    }

    PHI_ATTRIBUTE_PURE phi::u32 CodeEditor::GetCharacterColumn(phi::u32 line_number,
                                                               phi::u32 index) const noexcept
    {
        PHI_ASSERT(line_number < m_Lines.size());

        const Line& line   = m_Lines[line_number.unsafe()];
        phi::u32    column = 0u;
        phi::u32    i      = 0u;

        while (i < index && i < line.size())
        {
            char character = static_cast<char>(line[static_cast<phi::size_t>(i.unsafe())].m_Char);
            i += UTF8CharLength(character);
            if (character == '\t')
            {
                column = (column.unsafe() / m_TabSize.unsafe()) * m_TabSize.unsafe() +
                         m_TabSize.unsafe();
            }
            else
            {
                column++;
            }
        }

        return column;
    }

    PHI_ATTRIBUTE_PURE phi::u32 CodeEditor::GetLineCharacterCount(
            phi::u32 line_number) const noexcept
    {
        if (line_number >= m_Lines.size())
        {
            return 0u;
        }

        const Line& line  = m_Lines[line_number.unsafe()];
        phi::u32    count = 0u;

        for (phi::u32 i{0u}; i < line.size(); ++count)
        {
            i += UTF8CharLength(line[static_cast<phi::size_t>(i.unsafe())].m_Char);
        }

        return count;
    }

    PHI_ATTRIBUTE_PURE phi::u32 CodeEditor::GetLineMaxColumn(phi::u32 line_number) const noexcept
    {
        if (line_number >= m_Lines.size())
        {
            return 0u;
        }

        const Line& line = m_Lines[line_number.unsafe()];
        phi::u32    col  = 0u;

        for (phi::usize i{0u}; i < line.size();)
        {
            char character = static_cast<char>(line[i.unsafe()].m_Char);
            if (character == '\t')
            {
                col = (col / m_TabSize) * m_TabSize + m_TabSize;
            }
            else
            {
                col++;
            }

            i += UTF8CharLength(character);
        }

        return col;
    }

    PHI_ATTRIBUTE_PURE phi::u32 CodeEditor::GetMaxLineNumber() const noexcept
    {
        PHI_ASSERT(!m_Lines.empty());

        return static_cast<phi::uint32_t>(m_Lines.size() - 1u);
    }

    PHI_ATTRIBUTE_PURE phi::boolean CodeEditor::IsOnWordBoundary(
            const Coordinates& at) const noexcept
    {
        if (at.m_Line >= m_Lines.size() || at.m_Column == 0u)
        {
            return true;
        }

        PHI_ASSERT(at.m_Line < m_Lines.size());
        const Line& line   = m_Lines[at.m_Line.unsafe()];
        phi::u32    cindex = GetCharacterIndex(at);
        if (cindex >= line.size())
        {
            return true;
        }

        if (m_ColorizerEnabled)
        {
            return line[cindex.unsafe()].m_ColorIndex != line[cindex.unsafe() - 1u].m_ColorIndex;
        }

        PHI_ASSERT(cindex < line.size());
        return phi::is_space(static_cast<char>(line[cindex.unsafe()].m_Char)) !=
               phi::is_space(static_cast<char>(line[(cindex - 1u).unsafe()].m_Char));
    }

    void CodeEditor::RemoveLine(phi::u32 start, phi::u32 end) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(end >= start);
        PHI_ASSERT(m_Lines.size() > (end - start));

        // Remove error markers
        ErrorMarkers etmp;
        for (const auto& marker : m_ErrorMarkers)
        {
            if (marker.first >= start && marker.first <= end)
            {
                continue;
            }

            ErrorMarkers::value_type error_marker(
                    marker.first >= start ? phi::max(marker.first - (end - start + 1u), 1u) :
                                            marker.first,
                    marker.second);

            etmp.insert(error_marker);
        }
        m_ErrorMarkers = phi::move(etmp);

        // Remove breakpoints
        Breakpoints btmp;
        for (const auto line_number : m_Breakpoints)
        {
            if (line_number >= start && line_number <= end)
            {
                continue;
            }
            btmp.insert(line_number >= start ?
                                phi::max(line_number - (end - start + 1u).unsafe(), 1u) :
                                line_number);
        }
        m_Breakpoints = phi::move(btmp);

        m_Lines.erase(m_Lines.begin() + start.unsafe(), m_Lines.begin() + end.unsafe());
        PHI_ASSERT(!m_Lines.empty());

        // Fix selection state
        if (m_State.m_SelectionStart.m_Line >= start)
        {
            if (m_State.m_SelectionStart.m_Line < end - start)
            {
                m_State.m_SelectionStart.m_Line = 0u;
            }
            else
            {
                m_State.m_SelectionStart.m_Line -= end - start;
            }
        }
        if (m_State.m_SelectionEnd.m_Line >= start)
        {
            if (m_State.m_SelectionEnd.m_Line < end - start)
            {
                m_State.m_SelectionEnd.m_Line = 0u;
            }
            else
            {
                m_State.m_SelectionEnd.m_Line -= end - start;
            }
        }
        if (m_State.m_SelectionStart > m_State.m_SelectionEnd)
        {
            phi::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
        }

        m_TextChanged = true;
    }

    void CodeEditor::RemoveLine(phi::u32 index) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(m_Lines.size() > 1u);

        // Clear error markers on that line
        ErrorMarkers etmp;
        for (const auto& marker : m_ErrorMarkers)
        {
            ErrorMarkers::value_type error_marker(
                    marker.first > index ? phi::max(marker.first - 1u, 1u) : marker.first,
                    marker.second);
            if ((error_marker.first - 1u) == index)
            {
                continue;
            }
            etmp.insert(error_marker);
        }
        m_ErrorMarkers = phi::move(etmp);

        // Remove breakpoints on that line
        Breakpoints btmp;
        for (const auto line_number : m_Breakpoints)
        {
            if (line_number == index)
            {
                continue;
            }

            btmp.insert(line_number >= index ? phi::max(line_number - 1u, 1u) : line_number);
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
        PHI_ASSERT(!m_Lines.empty());

        m_TextChanged = true;
    }

    CodeEditor::Line& CodeEditor::InsertLine(phi::u32 index) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);

        Line& result = *m_Lines.insert(m_Lines.begin() + index.unsafe(), Line());

        ErrorMarkers etmp;
        for (const auto& marker : m_ErrorMarkers)
        {
            etmp.insert(ErrorMarkers::value_type(
                    marker.first >= index ? marker.first + 1u : marker.first, marker.second));
        }
        m_ErrorMarkers = phi::move(etmp);

        Breakpoints btmp;
        for (const phi::u32 line_number : m_Breakpoints)
        {
            btmp.insert(line_number >= index ? line_number.unsafe() + 1u : line_number.unsafe());
        }
        m_Breakpoints = phi::move(btmp);

        return result;
    }

    void CodeEditor::EnterCharacterImpl(ImWchar character, phi::boolean shift) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(IsValidUTF8Sequence(character));
        PHI_ASSERT(character != '\0');

        UndoRecord undo;
        undo.StoreBeforeState(this);

        if (HasSelection())
        {
            // Do indenting
            if (character == '\t' &&
                (m_State.m_SelectionStart.m_Column == 0u ||
                 m_State.m_SelectionStart.m_Line != m_State.m_SelectionEnd.m_Line))
            {
                Coordinates start        = m_State.m_SelectionStart;
                Coordinates end          = m_State.m_SelectionEnd;
                Coordinates original_end = end;

                PHI_ASSERT(start < end);
                start.m_Column = 0u;
                //          end.mColumn = end.mLine < mLines.size() ? mLines[end.mLine].size() : 0;
                if (end.m_Column == 0u && end.m_Line > 0u)
                {
                    --end.m_Line;
                }
                if (end.m_Line >= m_Lines.size())
                {
                    PHI_ASSERT(!m_Lines.empty());
                    end.m_Line = GetMaxLineNumber();
                }
                end.m_Column = GetLineMaxColumn(end.m_Line);

                //if (end.mColumn >= GetLineMaxColumn(end.mLine))
                //  end.mColumn = GetLineMaxColumn(end.mLine) - 1;

                undo.m_RemovedStart = start;
                undo.m_RemovedEnd   = end;
                undo.m_Removed      = GetText(start, end);

                phi::boolean modified = false;

                for (phi::u32 line_index = start.m_Line; line_index <= end.m_Line; ++line_index)
                {
                    PHI_ASSERT(line_index < m_Lines.size());
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
                            for (phi::u32 j = 0u;
                                 j < m_TabSize && !line.empty() && line.front().m_Char == ' '; ++j)
                            {
                                line.erase(line.begin());
                                modified = true;
                            }
                        }
                    }
                    else
                    {
                        PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wnull-dereference")
                        PHI_ASSERT(!line.empty());
                        PHI_ASSERT(line.data() != nullptr);

                        // Add indention
                        line.insert(line.begin(), Glyph('\t', PaletteIndex::Background));
                        modified = true;

                        PHI_GCC_SUPPRESS_WARNING_POP()
                    }
                }

                if (modified)
                {
                    start = Coordinates(start.m_Line, GetCharacterColumn(start.m_Line, 0u));
                    Coordinates range_end;

                    if (original_end.m_Column != 0u)
                    {
                        end          = Coordinates(end.m_Line, GetLineMaxColumn(end.m_Line));
                        range_end    = end;
                        undo.m_Added = GetText(start, end);
                    }
                    else
                    {
                        end       = Coordinates(original_end.m_Line, 0u);
                        range_end = Coordinates(end.m_Line - 1u, GetLineMaxColumn(end.m_Line - 1u));
                        undo.m_Added = GetText(start, range_end);
                    }

                    undo.m_AddedStart        = start;
                    undo.m_AddedEnd          = range_end;
                    m_State.m_SelectionStart = start;
                    m_State.m_SelectionEnd   = end;

                    undo.StoreAfterState(this);
                    AddUndo(undo);

                    m_TextChanged = true;

                    EnsureCursorVisible();
                }

                return;
            } // c == '\t'
            else
            {
                undo.m_Removed      = GetSelectedText();
                undo.m_RemovedStart = m_State.m_SelectionStart;
                undo.m_RemovedEnd   = m_State.m_SelectionEnd;
                DeleteSelection();
            }
        } // HasSelection

        Coordinates coord = GetActualCursorCoordinates();
        undo.m_AddedStart = coord;

        PHI_ASSERT(!m_Lines.empty());

        if (character == '\n')
        {
            InsertLine(coord.m_Line + 1u);
            undo.m_Added = static_cast<char>(character);

            PHI_ASSERT(coord.m_Line < m_Lines.size() - 1u);
            Line& line     = m_Lines[coord.m_Line.unsafe()];
            Line& new_line = m_Lines[coord.m_Line.unsafe() + 1u];

            for (phi::u32 it{0u}; it < line.size() && it < coord.m_Column &&
                                  phi::is_blank(static_cast<char>(line[it.unsafe()].m_Char));
                 ++it)
            {
                new_line.push_back(line[it.unsafe()]);
                undo.m_Added += static_cast<char>(line[it.unsafe()].m_Char);
            }

            const phi::usize whitespace_size = new_line.size();
            const phi::u32   cindex          = GetCharacterIndex(coord);
            new_line.insert(new_line.end(), line.begin() + cindex.unsafe(), line.end());
            line.erase(line.begin() + cindex.unsafe(),
                       line.begin() + static_cast<phi::int32_t>(line.size()));

            SetCursorPosition(Coordinates(
                    coord.m_Line + 1u,
                    GetCharacterColumn(coord.m_Line.unsafe() + 1u,
                                       static_cast<phi::uint32_t>(whitespace_size.unsafe()))));

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
            PHI_ASSERT(length > 0u);

            PHI_ASSERT(coord.m_Line < m_Lines.size());
            Line&    line   = m_Lines[static_cast<phi::size_t>(coord.m_Line.unsafe())];
            phi::u32 cindex = GetCharacterIndex(coord);

            if (m_Overwrite && cindex < line.size())
            {
                phi::u8_fast char_length = UTF8CharLength(line[cindex.unsafe()].m_Char);

                undo.m_RemovedStart = m_State.m_CursorPosition;
                undo.m_RemovedEnd   = Coordinates(
                        coord.m_Line, GetCharacterColumn(coord.m_Line, cindex + char_length));

                for (; char_length > 0u && cindex < line.size(); --char_length)
                {
                    undo.m_Removed += static_cast<char>(line[cindex.unsafe()].m_Char);
                    line.erase(line.begin() + cindex.unsafe());
                }
            }

            for (char* pointer = buffer.data(); *pointer != '\0'; ++pointer, ++cindex)
            {
                line.insert(line.begin() + cindex.unsafe(),
                            Glyph(static_cast<phi::uint8_t>(*pointer), PaletteIndex::Default));
            }
            undo.m_Added = std::string_view{buffer.data(), length.unsafe()};

            SetCursorPosition(Coordinates(coord.m_Line, GetCharacterColumn(coord.m_Line, cindex)));
        }

        m_TextChanged = true;

        undo.m_AddedEnd = GetActualCursorCoordinates();
        undo.StoreAfterState(this);

        AddUndo(undo);

        Colorize(coord.m_Line, 1u);
    }

    void CodeEditor::BackspaceImpl() noexcept
    {
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(!m_Lines.empty());

        UndoRecord undo;
        undo.StoreBeforeState(this);

        if (HasSelection())
        {
            undo.m_Removed      = GetSelectedText();
            undo.m_RemovedStart = m_State.m_SelectionStart;
            undo.m_RemovedEnd   = m_State.m_SelectionEnd;

            DeleteSelection();
        }
        else
        {
            Coordinates pos = GetActualCursorCoordinates();
            SetCursorPosition(pos);

            if (m_State.m_CursorPosition.m_Column == 0u)
            {
                if (m_State.m_CursorPosition.m_Line == 0u)
                {
                    return;
                }

                undo.m_Removed      = '\n';
                undo.m_RemovedStart = undo.m_RemovedEnd =
                        Coordinates(pos.m_Line - 1u, GetLineMaxColumn(pos.m_Line - 1u));
                Advance(undo.m_RemovedEnd);

                PHI_ASSERT(m_State.m_CursorPosition.m_Line < m_Lines.size());
                Line& line      = m_Lines[m_State.m_CursorPosition.m_Line.unsafe()];
                Line& prev_line = m_Lines[m_State.m_CursorPosition.m_Line.unsafe() - 1u];

                const phi::u32 prev_size = GetLineMaxColumn(m_State.m_CursorPosition.m_Line - 1u);
                prev_line.insert(prev_line.end(), line.begin(), line.end());

                ErrorMarkers etmp;
                for (auto& error_marker : m_ErrorMarkers)
                {
                    const phi::u32 error_marker_line = error_marker.first;

                    etmp.insert(ErrorMarkers::value_type(
                            (error_marker_line - 1u) == m_State.m_CursorPosition.m_Line ?
                                    error_marker_line - 1u :
                                    error_marker_line,
                            error_marker.second));
                }
                m_ErrorMarkers = phi::move(etmp);

                RemoveLine(m_State.m_CursorPosition.m_Line);
                --m_State.m_CursorPosition.m_Line;
                m_State.m_CursorPosition.m_Column = prev_size;
            }
            else
            {
                PHI_ASSERT(m_State.m_CursorPosition.m_Line < m_Lines.size());
                Line&    line   = m_Lines[m_State.m_CursorPosition.m_Line.unsafe()];
                phi::u32 cindex = GetCharacterIndex(pos) - 1u;

                if (line[cindex.unsafe()].m_Char == '\t')
                {
                    undo.m_RemovedStart.m_Line = GetActualCursorCoordinates().m_Line;
                    undo.m_RemovedEnd          = GetActualCursorCoordinates();
                    undo.m_Removed             = '\t';

                    line.erase(line.begin() + cindex.unsafe());

                    // Move cursor back
                    m_State.m_CursorPosition.m_Column = GetCharacterColumn(pos.m_Line, cindex);
                    undo.m_RemovedStart.m_Column      = GetCharacterColumn(pos.m_Line, cindex);
                }
                else
                {
                    phi::u32 cend = cindex + 1u;
                    while (cindex > 0u && IsUTFSequence(line[cindex.unsafe()].m_Char))
                    {
                        --cindex;
                    }

                    undo.m_RemovedStart = undo.m_RemovedEnd = GetActualCursorCoordinates();
                    --undo.m_RemovedStart.m_Column;
                    --m_State.m_CursorPosition.m_Column;

                    for (; cindex < line.size() && cend > cindex; --cend)
                    {
                        undo.m_Removed += static_cast<char>(line[cindex.unsafe()].m_Char);
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

        undo.StoreAfterState(this);
        AddUndo(undo);
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

    PHI_ATTRIBUTE_PURE ImU32 CodeEditor::GetGlyphColor(const Glyph& glyph) const noexcept
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

        ImGuiIO&           imgui_io = ImGui::GetIO();
        const phi::boolean shift    = imgui_io.KeyShift;
        const phi::boolean ctrl =
                imgui_io.ConfigMacOSXBehaviors ? imgui_io.KeySuper : imgui_io.KeyCtrl;
        const phi::boolean alt =
                imgui_io.ConfigMacOSXBehaviors ? imgui_io.KeyCtrl : imgui_io.KeyAlt;

        if (ImGui::IsWindowHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
        }
        //ImGui::CaptureKeyboardFromApp(true);

        imgui_io.WantCaptureKeyboard = true;
        imgui_io.WantTextInput       = true;

        if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Z))
        {
            Undo();
        }
        else if (!IsReadOnly() && !ctrl && !shift && alt && ImGui::IsKeyPressed(ImGuiKey_Backspace))
        {
            Undo();
        }
        else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Y))
        {
            Redo();
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        {
            MoveUp(1u, shift);
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
        {
            MoveDown(1u, shift);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
        {
            MoveLeft(1u, shift, ctrl);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGuiKey_RightArrow))
        {
            MoveRight(1u, shift, ctrl);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGuiKey_PageUp))
        {
            const phi::u32 page_size = GetPageSize();
            MoveUp(page_size - 4u, shift);
        }
        else if (!alt && ImGui::IsKeyPressed(ImGuiKey_PageDown))
        {
            const phi::u32 page_size = GetPageSize();
            MoveDown(page_size - 4u, shift);
        }
        else if (!alt && ctrl && ImGui::IsKeyPressed(ImGuiKey_Home))
        {
            MoveTop(shift);
        }
        else if (ctrl && !alt && ImGui::IsKeyPressed(ImGuiKey_End))
        {
            MoveBottom(shift);
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGuiKey_Home))
        {
            MoveHome(shift);
        }
        else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGuiKey_End))
        {
            MoveEnd(shift);
        }
        else if (!IsReadOnly() && !ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Delete))
        {
            Delete();
        }
        else if (!IsReadOnly() && !ctrl && !shift && !alt &&
                 ImGui::IsKeyPressed(ImGuiKey_Backspace))
        {
            BackspaceImpl();
        }
        else if (!ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Insert))
        {
            m_Overwrite = !m_Overwrite;
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Insert))
        {
            Copy();
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_C))
        {
            Copy();
        }
        else if (!IsReadOnly() && !ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Insert))
        {
            Paste();
        }
        else if (!IsReadOnly() && ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_V))
        {
            Paste();
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_X))
        {
            Cut();
        }
        else if (!ctrl && shift && !alt && ImGui::IsKeyPressed(ImGuiKey_Delete))
        {
            Cut();
        }
        else if (ctrl && !shift && !alt && ImGui::IsKeyPressed(ImGuiKey_A))
        {
            SelectAll();
        }
        else if (!IsReadOnly() && !ctrl && !alt &&
                 (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)))
        {
            EnterCharacterImpl('\n', shift);
        }
        else if (!IsReadOnly() && !ctrl && !alt && ImGui::IsKeyPressed(ImGuiKey_Tab))
        {
            EnterCharacterImpl('\t', shift);
        }

        if (!IsReadOnly() && !imgui_io.InputQueueCharacters.empty())
        {
            for (phi::i32 i{0}; i < imgui_io.InputQueueCharacters.Size; ++i)
            {
                ImWchar input_char = imgui_io.InputQueueCharacters[i.unsafe()];
                if (input_char != '\0' && IsValidUTF8Sequence(input_char))
                {
                    EnterCharacterImpl(input_char, shift);
                }
            }

            imgui_io.InputQueueCharacters.resize(0);
        }
    }

    void CodeEditor::HandleMouseInputs() noexcept
    {
        // Do nothing if our window is not hovered
        if (!ImGui::IsWindowHovered())
        {
            return;
        }

        ImGuiIO&           imgui_io = ImGui::GetIO();
        const phi::boolean shift    = imgui_io.KeyShift;
        const phi::boolean ctrl =
                imgui_io.ConfigMacOSXBehaviors ? imgui_io.KeySuper : imgui_io.KeyCtrl;
        const phi::boolean alt =
                imgui_io.ConfigMacOSXBehaviors ? imgui_io.KeyCtrl : imgui_io.KeyAlt;

        if (shift || alt)
        {
            return;
        }

        // TODO: Maybe theres a smarter way here than suppressing the warning and comparing m_LastClick != -1.0f?
        PHI_CLANG_AND_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wfloat-equal")

        const phi::boolean click        = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
        const phi::boolean double_click = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        const double       time         = ImGui::GetTime();
        const phi::boolean triple_click =
                click && !double_click &&
                (m_LastClick != -1.0f &&
                 (static_cast<float>(time) - m_LastClick) < imgui_io.MouseDoubleClickTime);

        PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

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

            m_LastClick = static_cast<float>(ImGui::GetTime());
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

            m_LastClick = static_cast<float>(ImGui::GetTime());
        }
        // Mouse left button dragging (=> update selection)
        else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) &&
                 ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            imgui_io.WantCaptureMouse = true;
            m_State.m_CursorPosition  = m_InteractiveEnd =
                    ScreenPosToCoordinates(ImGui::GetMousePos());
            SetSelection(m_InteractiveStart, m_InteractiveEnd, m_SelectionMode);
        }
    }

    // TODO: This function is a real mess Split it into multiple sub functions for the different parts maybe?
    void CodeEditor::InternalRender() noexcept
    {
        PHI_ASSERT(m_LineBuffer.empty());

        const ImVec2 content_size = ImGui::GetWindowContentRegionMax();
        ImDrawList*  draw_list    = ImGui::GetWindowDrawList();
        float        longest      = m_TextStart;

        if (m_ScrollToTop)
        {
            m_ScrollToTop = false;
            ImGui::SetScrollY(0.f);
        }

        const ImVec2 cursor_screen_pos = ImGui::GetCursorScreenPos();
        const float  scroll_x          = ImGui::GetScrollX();
        const float  scroll_y          = ImGui::GetScrollY();

        PHI_ASSERT(m_CharAdvance.y != 0.0f);
        phi::u32       line_no = static_cast<phi::uint32_t>(std::floor(scroll_y / m_CharAdvance.y));
        const phi::u32 global_line_max = static_cast<phi::uint32_t>(m_Lines.size());

        // TODO: This is very unreadable
        PHI_ASSERT(m_CharAdvance.y != 0.0f);
        const phi::u32 line_max = static_cast<phi::uint32_t>(phi::clamp(
                GetMaxLineNumber().unsafe(), 0u,
                line_no.unsafe() + static_cast<phi::int32_t>(std::floor(
                                           (scroll_y + content_size.y) / m_CharAdvance.y))));

        // Deduce m_TextStart by evaluating mLines size (global lineMax) plus two spaces as text width
        phi::array<char, 16u> buffer;
        snprintf(buffer.data(), buffer.size().unsafe(), " %u ", global_line_max.unsafe());
        m_TextStart = ImGui::GetFont()
                              ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buffer.data(),
                                              nullptr, nullptr)
                              .x +
                      LeftMargin;

        PHI_ASSERT(!m_Lines.empty());
        float space_size =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr)
                        .x;

        while (line_no <= line_max)
        {
            const ImVec2 line_start_screen_pos = ImVec2(
                    cursor_screen_pos.x,
                    cursor_screen_pos.y + static_cast<float>(line_no.unsafe()) * m_CharAdvance.y);
            const ImVec2 text_screen_pos =
                    ImVec2(line_start_screen_pos.x + m_TextStart, line_start_screen_pos.y);

            PHI_ASSERT(line_no < m_Lines.size());
            const Line& line = m_Lines[line_no.unsafe()];

            longest               = phi::max(m_TextStart + TextDistanceToLineStart(Coordinates(
                                                     line_no, GetLineMaxColumn(line_no))),
                                             longest);
            phi::u32    column_no = 0u;
            Coordinates line_start_coord(line_no, 0u);
            Coordinates line_end_coord(line_no, GetLineMaxColumn(line_no));

            // Draw selection for the current line
            float sstart = -1.0f;
            float ssend  = -1.0f;

            PHI_ASSERT(m_State.m_SelectionStart <= m_State.m_SelectionEnd);
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

            PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wfloat-equal")

            if (sstart != -1.0f && ssend != -1.0f && sstart < ssend)
            {
                const ImVec2 vstart(line_start_screen_pos.x + m_TextStart + sstart,
                                    line_start_screen_pos.y);
                const ImVec2 vend(line_start_screen_pos.x + m_TextStart + ssend,
                                  line_start_screen_pos.y + m_CharAdvance.y);

                draw_list->AddRectFilled(vstart, vend, GetPaletteForIndex(PaletteIndex::Selection));
            }

            PHI_GCC_SUPPRESS_WARNING_POP()

            // Draw breakpoints
            const ImVec2 start =
                    ImVec2(line_start_screen_pos.x + scroll_x, line_start_screen_pos.y);

            if (m_Breakpoints.contains(line_no.unsafe() + 1u))
            {
                const ImVec2 end =
                        ImVec2(line_start_screen_pos.x + content_size.x + 2.0f * scroll_x,
                               line_start_screen_pos.y + m_CharAdvance.y);

                draw_list->AddRectFilled(start, end, GetPaletteForIndex(PaletteIndex::Breakpoint));
            }

            // Draw error markers
            auto error_it = m_ErrorMarkers.find(line_no.unsafe() + 1u);
            if (error_it != m_ErrorMarkers.end())
            {
                const ImVec2 end =
                        ImVec2(line_start_screen_pos.x + content_size.x + 2.0f * scroll_x,
                               line_start_screen_pos.y + m_CharAdvance.y);
                draw_list->AddRectFilled(start, end, GetPaletteForIndex(PaletteIndex::ErrorMarker));

                if (GImGui->HoveredWindow == ImGui::GetCurrentWindow() &&
                    ImGui::IsMouseHoveringRect(line_start_screen_pos, end))
                {
                    ImGui::BeginTooltip();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                    ImGui::Text("Error at line %u:", error_it->first);
                    ImGui::PopStyleColor();
                    ImGui::Separator();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
                    ImGui::Text("%s", error_it->second.c_str());
                    ImGui::PopStyleColor();
                    ImGui::EndTooltip();
                }
            }

            // Highlight PC line
            const phi::u64 current_execution_line_number = m_Emulator->GetExecutingLineNumber();
            if (line_no + 1u == current_execution_line_number)
            {
                const ImVec2 end =
                        ImVec2(line_start_screen_pos.x + content_size.x + 2.0f * scroll_x,
                               line_start_screen_pos.y + m_CharAdvance.y);
                // TODO: Don't hardcode color
                draw_list->AddRectFilled(start, end, 0x40ffff00);
            }

            // Draw line number (right aligned)
            snprintf(buffer.data(), buffer.size().unsafe(), "%u  ", line_no.unsafe() + 1u);

            const float line_no_width =
                    ImGui::GetFont()
                            ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buffer.data(),
                                            nullptr, nullptr)
                            .x;
            draw_list->AddText(ImVec2(line_start_screen_pos.x + m_TextStart - line_no_width,
                                      line_start_screen_pos.y),
                               GetPaletteForIndex(PaletteIndex::LineNumber), buffer.data());

            if (m_State.m_CursorPosition.m_Line == line_no)
            {
                const phi::boolean focused = ImGui::IsWindowFocused();

                // Highlight the current line (where the cursor is)
                if (!HasSelection())
                {
                    const ImVec2 end =
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
                    phi::uint64_t time_end = static_cast<phi::uint64_t>(
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                    std::chrono::system_clock::now().time_since_epoch())
                                    .count());
                    phi::uint64_t elapsed = time_end - m_StartTime;
                    if (elapsed > 400)
                    {
                        float    width  = 1.0f;
                        phi::u32 cindex = GetCharacterIndex(m_State.m_CursorPosition);
                        float    cx     = TextDistanceToLineStart(m_State.m_CursorPosition);

                        if (m_Overwrite && cindex < line.size())
                        {
                            char character = static_cast<char>(line[cindex.unsafe()].m_Char);
                            if (character == '\t')
                            {
                                float x = (1.0f +
                                           std::floor((1.0f + cx) /
                                                      (float(m_TabSize.unsafe()) * space_size))) *
                                          (float(m_TabSize.unsafe()) * space_size);
                                width = x - cx;
                            }
                            else
                            {
                                phi::array<char, 2u> buffer2;
                                buffer2[0u] = static_cast<char>(line[cindex.unsafe()].m_Char);
                                buffer2[1u] = '\0';
                                width       = ImGui::GetFont()
                                                ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX,
                                                                -1.0f, buffer2.data())
                                                .x;
                            }
                        }
                        const ImVec2 cstart(text_screen_pos.x + cx, line_start_screen_pos.y);
                        const ImVec2 cend(text_screen_pos.x + cx + width,
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

            for (phi::usize i{0u}; i < line.size();)
            {
                const CodeEditor::Glyph& glyph = line[i.unsafe()];
                const ImU32              color = GetGlyphColor(glyph);

                if ((color != prev_color || glyph.m_Char == '\t' || glyph.m_Char == ' ') &&
                    !m_LineBuffer.empty())
                {
                    const ImVec2 new_offset(text_screen_pos.x + buffer_offset.x,
                                            text_screen_pos.y + buffer_offset.y);
                    draw_list->AddText(new_offset, prev_color, m_LineBuffer.c_str());
                    const ImVec2 text_size =
                            ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f,
                                                            m_LineBuffer.c_str(), nullptr, nullptr);
                    buffer_offset.x += text_size.x;
                    m_LineBuffer.clear();
                }
                prev_color = color;

                if (glyph.m_Char == '\t')
                {
                    const float old_x = buffer_offset.x;
                    buffer_offset.x =
                            (1.0f + std::floor((1.0f + buffer_offset.x) /
                                               (float(m_TabSize.unsafe()) * space_size))) *
                            (float(m_TabSize.unsafe()) * space_size);
                    ++i;

                    if (m_ShowWhitespaces)
                    {
                        const float font_size =
                                ImGui::GetFont()
                                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#",
                                                        nullptr, nullptr)
                                        .x;

                        const float  s  = ImGui::GetFontSize();
                        const float  x1 = text_screen_pos.x + old_x + 1.0f;
                        const float  x2 = text_screen_pos.x + buffer_offset.x - 1.0f;
                        const float  y  = text_screen_pos.y + buffer_offset.y + s * 0.5f;
                        const ImVec2 p1(x1, y);
                        const ImVec2 p2(x2, y);
                        const ImVec2 p3(x2 - font_size * 0.2f, y - font_size * 0.2f);
                        const ImVec2 p4(x2 - font_size * 0.2f, y + font_size * 0.2f);

                        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5264) // Unused const variable

                        static constexpr const ImU32 whitespace_color{0x90909090};

                        PHI_MSVC_SUPPRESS_WARNING_POP()

                        draw_list->AddLine(p1, p2, whitespace_color);
                        draw_list->AddLine(p2, p3, whitespace_color);
                        draw_list->AddLine(p2, p4, whitespace_color);
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
                    for (phi::u8_fast length = UTF8CharLength(glyph.m_Char); length > 0u;
                         ++i, --length)
                    {
                        m_LineBuffer.push_back(static_cast<char>(line[i.unsafe()].m_Char));
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

        ImGui::Dummy(ImVec2((longest + 2), static_cast<float>(m_Lines.size()) * m_CharAdvance.y));

        if (m_ScrollToCursor)
        {
            ScrollToCursor();
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

        PHI_ASSERT(token.GetLineNumber() - 1u < m_Lines.size());
        Line& line = m_Lines[(token.GetLineNumber() - 1u).unsafe()];

        for (phi::u64 index{token.GetColumn() - 1u};
             index < token.GetColumn() + token.GetLength() - 1u; ++index)
        {
            PHI_ASSERT(index < line.size());
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
        m_State.m_CursorPosition = Coordinates(0u, 0u);
        m_State.m_SelectionStart = Coordinates(0u, 0u);
        m_State.m_SelectionEnd   = Coordinates(0u, 0u);

        // Clear breakpoints and markers
        ClearBreakPoints();
        ClearErrorMarkers();
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

    PHI_ATTRIBUTE_PURE phi::u8_fast CodeEditor::GetTabSizeAt(phi::u32 column) const noexcept
    {
        return static_cast<phi::uint_fast8_t>(m_TabSize.unsafe() -
                                              (column.unsafe() % m_TabSize.unsafe()));
    }

    PHI_ATTRIBUTE_PURE ImU32 CodeEditor::GetPaletteForIndex(PaletteIndex index) const noexcept
    {
        PHI_ASSERT(index != PaletteIndex::Max);

        const phi::size_t int_value = static_cast<phi::size_t>(index);
        PHI_ASSERT(int_value < m_Palette.size());

        return m_Palette[int_value];
    }

    void CodeEditor::ComputeCharAdvance() noexcept
    {
        // Compute m_CharAdvance regarding to scaled font size (Ctrl + mouse wheel)
        const float font_size =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr)
                        .x;
        m_CharAdvance = ImVec2(font_size, ImGui::GetTextLineHeightWithSpacing() * m_LineSpacing);

        PHI_ASSERT(m_CharAdvance.x != 0.0f);
        PHI_ASSERT(m_CharAdvance.y != 0.0f);
    }
} // namespace dlxemu
