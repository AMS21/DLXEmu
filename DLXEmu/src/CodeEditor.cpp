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
#include <Phi/Core/Assert.hpp>
#include <Phi/Core/Log.hpp>
#include <magic_enum.hpp>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h> // for imGui::GetCurrentWindow()

namespace dlxemu
{
    CodeEditor::CodeEditor(Emulator* emulator) noexcept
        : m_LineSpacing(1.0f)
        , m_UndoIndex(0)
        , m_TabSize(4)
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

    void CodeEditor::SetPalette(const Palette& value) noexcept
    {
        m_PaletteBase = value;
    }

    std::string CodeEditor::GetText(const Coordinates& start, const Coordinates& end) const noexcept
    {
        std::string result;

        std::int32_t lstart = start.m_Line;
        std::int32_t lend   = end.m_Line;
        std::int32_t istart = GetCharacterIndex(start);
        std::int32_t iend   = GetCharacterIndex(end);
        std::size_t  s      = 0;

        for (std::size_t i = lstart; i < lend; i++)
        {
            s += m_Lines[i].size();
        }

        result.reserve(s + s / 8);

        while (istart < iend || lstart < lend)
        {
            if (lstart >= (std::int32_t)m_Lines.size())
            {
                break;
            }

            const Line& line = m_Lines[lstart];
            if (istart < (std::int32_t)line.size())
            {
                result += line[istart].m_Char;
                istart++;
            }
            else
            {
                istart = 0;
                ++lstart;
                result += '\n';
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
        std::int32_t line   = value.m_Line;
        std::int32_t column = value.m_Column;

        if (line >= (std::int32_t)m_Lines.size())
        {
            if (m_Lines.empty())
            {
                line   = 0;
                column = 0;
            }
            else
            {
                line   = (std::int32_t)m_Lines.size() - 1;
                column = GetLineMaxColumn(line);
            }

            return Coordinates(line, column);
        }

        column = m_Lines.empty() ? 0 : std::max(column, GetLineMaxColumn(line));
        line   = std::max(0, line);

        return Coordinates(line, column);
    }

    // https://en.wikipedia.org/wiki/UTF-8
    // We assume that the char is a standalone character (<128) or a leading byte of an UTF-8 code sequence (non-10xxxxxx code)
    static std::int32_t UTF8CharLength(char c) noexcept
    {
        if ((c & 0xFE) == 0xFC)
        {
            return 6;
        }

        if ((c & 0xFC) == 0xF8)
        {
            return 5;
        }

        if ((c & 0xF8) == 0xF0)
        {
            return 4;
        }

        if ((c & 0xF0) == 0xE0)
        {
            return 3;
        }

        if ((c & 0xE0) == 0xC0)
        {
            return 2;
        }

        return 1;
    }

    // "Borrowed" from ImGui source
    static inline std::int32_t ImTextCharToUtf8(char* buf, std::int32_t buf_size,
                                                std::uint32_t c) noexcept
    {
        if (c < 0x80)
        {
            buf[0] = (char)c;
            return 1;
        }
        if (c < 0x800)
        {
            if (buf_size < 2)
            {
                return 0;
            }

            buf[0] = (char)(0xc0 + (c >> 6));
            buf[1] = (char)(0x80 + (c & 0x3f));

            return 2;
        }
        if (c >= 0xdc00 && c < 0xe000)
        {
            return 0;
        }

        if (c >= 0xd800 && c < 0xdc00)
        {
            if (buf_size < 4)
            {
                return 0;
            }

            buf[0] = (char)(0xf0 + (c >> 18));
            buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
            buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
            buf[3] = (char)(0x80 + ((c)&0x3f));

            return 4;
        }
        //else if (c < 0x10000)
        {
            if (buf_size < 3)
            {
                return 0;
            }

            buf[0] = (char)(0xe0 + (c >> 12));
            buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
            buf[2] = (char)(0x80 + ((c)&0x3f));

            return 3;
        }
    }

    void CodeEditor::Advance(Coordinates& coordinates) const noexcept
    {
        if (coordinates.m_Line < (std::int32_t)m_Lines.size())
        {
            const Line&  line   = m_Lines[coordinates.m_Line];
            std::int32_t cindex = GetCharacterIndex(coordinates);

            if (cindex + 1 < (std::int32_t)line.size())
            {
                std::int32_t delta = UTF8CharLength(line[cindex].m_Char);
                cindex             = std::min(cindex + delta, (std::int32_t)line.size() - 1);
            }
            else
            {
                ++coordinates.m_Line;
                cindex = 0;
            }
            coordinates.m_Column = GetCharacterColumn(coordinates.m_Line, cindex);
        }
    }

    void CodeEditor::DeleteRange(const Coordinates& start, const Coordinates& end) noexcept
    {
        PHI_ASSERT(end >= start);
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(start.m_Line < m_Lines.size());
        PHI_ASSERT(end.m_Line < m_Lines.size());

        if (end == start)
        {
            return;
        }

        std::int32_t start_index = GetCharacterIndex(start);
        std::int32_t end_index   = GetCharacterIndex(end);

        if (start.m_Line == end.m_Line)
        {
            Line&        line = m_Lines[start.m_Line];
            std::int32_t n    = GetLineMaxColumn(start.m_Line);

            if (end.m_Column >= n)
            {
                line.erase(line.begin() + start_index, line.end());
            }
            else
            {
                line.erase(line.begin() + start_index, line.begin() + end_index);
            }
        }
        else
        {
            Line& first_line = m_Lines[start.m_Line];
            Line& last_line  = m_Lines[end.m_Line];

            first_line.erase(first_line.begin() + start_index, first_line.end());
            last_line.erase(last_line.begin(), last_line.begin() + end_index);

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

    std::int32_t CodeEditor::InsertTextAt(Coordinates& /* inout */ where,
                                          const char*              value) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);

        std::int32_t cindex      = GetCharacterIndex(where);
        std::int32_t total_lines = 0;
        while (*value != '\0')
        {
            PHI_ASSERT(!m_Lines.empty());

            if (*value == '\r')
            {
                // skip
                ++value;
            }
            else if (*value == '\n')
            {
                if (cindex < (std::int32_t)m_Lines[where.m_Line].size())
                {
                    Line& new_line = InsertLine(where.m_Line + 1);
                    Line& line     = m_Lines[where.m_Line];
                    new_line.insert(new_line.begin(), line.begin() + cindex, line.end());
                    line.erase(line.begin() + cindex, line.end());
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
            else
            {
                Line&        line = m_Lines[where.m_Line];
                std::int32_t d    = UTF8CharLength(*value);

                while (d-- > 0 && *value != '\0')
                {
                    line.insert(line.begin() + cindex++, Glyph(*value++, PaletteIndex::Default));
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

        m_UndoBuffer.resize(m_UndoIndex + 1u);
        m_UndoBuffer.back() = value;
        ++m_UndoIndex;
    }

    CodeEditor::Coordinates CodeEditor::ScreenPosToCoordinates(
            const ImVec2& position) const noexcept
    {
        ImVec2 origin = ImGui::GetCursorScreenPos();
        ImVec2 local(position.x - origin.x, position.y - origin.y);

        std::int32_t line_no = std::max(0, (std::int32_t)floor(local.y / m_CharAdvance.y));

        std::int32_t column_coord = 0;

        if (line_no >= 0 && line_no < (std::int32_t)m_Lines.size())
        {
            const Line& line = m_Lines.at(line_no);

            std::int32_t column_index = 0;
            float        column_x     = 0.0f;

            while ((size_t)column_index < line.size())
            {
                float column_width = 0.0f;

                if (line[column_index].m_Char == '\t')
                {
                    float space_size =
                            ImGui::GetFont()
                                    ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ")
                                    .x;
                    float old_x        = column_x;
                    float new_column_x = (1.0f + std::floor((1.0f + column_x) /
                                                            (float(m_TabSize) * space_size))) *
                                         (float(m_TabSize) * space_size);
                    column_width = new_column_x - old_x;

                    if (m_TextStart + column_x + column_width * 0.5f > local.x)
                    {
                        break;
                    }

                    column_x     = new_column_x;
                    column_coord = (column_coord / m_TabSize) * m_TabSize + m_TabSize;
                    column_index++;
                }
                else
                {
                    char         buf[7];
                    std::int32_t d = UTF8CharLength(line[column_index].m_Char);
                    std::int32_t i = 0;
                    while (i < 6 && d-- > 0)
                    {
                        buf[i++] = line[column_index++].m_Char;
                    }
                    buf[i] = '\0';
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
        Coordinates at = from;
        if (at.m_Line >= (std::int32_t)m_Lines.size())
        {
            return at;
        }

        const Line&  line   = m_Lines[at.m_Line];
        std::int32_t cindex = GetCharacterIndex(at);

        if (cindex >= (std::int32_t)line.size())
        {
            return at;
        }

        while (cindex > 0 && dlx::IsSpace(line[cindex].m_Char))
        {
            --cindex;
        }

        const PaletteIndex cstart = line[cindex].m_ColorIndex;
        while (cindex > 0)
        {
            Glyph c = line[cindex];
            if ((c.m_Char & 0xC0) != 0x80) // not UTF code sequence 10xxxxxx
            {
                if (c.m_Char <= 32 && dlx::IsSpace(c.m_Char))
                {
                    cindex++;
                    break;
                }

                if (cstart != line[size_t(cindex - 1)].m_ColorIndex)
                {
                    break;
                }
            }

            --cindex;
        }
        return Coordinates(at.m_Line, GetCharacterColumn(at.m_Line, cindex));
    }

    CodeEditor::Coordinates CodeEditor::FindWordEnd(const Coordinates& from) const noexcept
    {
        Coordinates at = from;
        if (at.m_Line >= (std::int32_t)m_Lines.size())
        {
            return at;
        }

        const Line&  line   = m_Lines[at.m_Line];
        std::int32_t cindex = GetCharacterIndex(at);

        if (cindex >= (std::int32_t)line.size())
        {
            return at;
        }

        bool         prevspace = dlx::IsSpace(line[cindex].m_Char);
        PaletteIndex cstart    = line[cindex].m_ColorIndex;
        while (cindex < (std::int32_t)line.size())
        {
            Glyph        c = line[cindex];
            std::int32_t d = UTF8CharLength(c.m_Char);

            if (cstart != line[cindex].m_ColorIndex)
            {
                break;
            }

            if (prevspace != dlx::IsSpace(c.m_Char))
            {
                if (dlx::IsSpace(c.m_Char))
                {
                    while (cindex < (std::int32_t)line.size() && dlx::IsSpace(line[cindex].m_Char))
                    {
                        ++cindex;
                    }
                }
                break;
            }

            cindex += d;
        }

        return Coordinates(from.m_Line, GetCharacterColumn(from.m_Line, cindex));
    }

    CodeEditor::Coordinates CodeEditor::FindNextWord(const Coordinates& from) const noexcept
    {
        Coordinates at = from;
        if (at.m_Line >= (std::int32_t)m_Lines.size())
        {
            return at;
        }

        // skip to the next non-word character
        std::int32_t cindex  = GetCharacterIndex(from);
        bool         is_word = false;
        bool         skip    = false;
        if (cindex < (std::int32_t)m_Lines[at.m_Line].size())
        {
            const Line& line = m_Lines[at.m_Line];
            is_word          = dlx::IsAlphaNumeric(line[cindex].m_Char);
            skip             = is_word;
        }

        while (!is_word || skip)
        {
            if (at.m_Line >= m_Lines.size())
            {
                std::int32_t l = std::max(0, (std::int32_t)m_Lines.size() - 1);

                return Coordinates(l, GetLineMaxColumn(l));
            }

            const Line& line = m_Lines[at.m_Line];
            if (cindex < (std::int32_t)line.size())
            {
                is_word = dlx::IsAlphaNumeric(line[cindex].m_Char);

                if (is_word && !skip)
                {
                    return Coordinates(at.m_Line, GetCharacterColumn(at.m_Line, cindex));
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
                ++at.m_Line;
                skip    = false;
                is_word = false;
            }
        }

        return at;
    }

    std::int32_t CodeEditor::GetCharacterIndex(const Coordinates& coordinates) const noexcept
    {
        if (coordinates.m_Line >= m_Lines.size())
        {
            return -1;
        }

        const Line&  line = m_Lines[coordinates.m_Line];
        std::int32_t c    = 0;
        std::int32_t i    = 0;
        for (; i < line.size() && c < coordinates.m_Column;)
        {
            if (line[i].m_Char == '\t')
            {
                c = (c / m_TabSize) * m_TabSize + m_TabSize;
            }
            else
            {
                ++c;
            }

            i += UTF8CharLength(line[i].m_Char);
        }

        return i;
    }

    std::int32_t CodeEditor::GetCharacterColumn(std::int32_t line_number,
                                                std::int32_t index) const noexcept
    {
        if (line_number >= m_Lines.size())
        {
            return 0;
        }

        const Line&  line = m_Lines[line_number];
        std::int32_t col  = 0;
        std::int32_t i    = 0;

        while (i < index && i < (std::int32_t)line.size())
        {
            char c = line[i].m_Char;
            i += UTF8CharLength(c);
            if (c == '\t')
            {
                col = (col / m_TabSize) * m_TabSize + m_TabSize;
            }
            else
            {
                col++;
            }
        }

        return col;
    }

    std::int32_t CodeEditor::GetLineCharacterCount(std::int32_t line_number) const noexcept
    {
        if (line_number >= m_Lines.size())
        {
            return 0;
        }

        const Line&  line = m_Lines[line_number];
        std::int32_t c    = 0;

        for (std::int32_t i = 0; i < line.size(); c++)
        {
            i += UTF8CharLength(line[i].m_Char);
        }

        return c;
    }

    std::int32_t CodeEditor::GetLineMaxColumn(std::int32_t line_number) const noexcept
    {
        if (line_number >= m_Lines.size())
        {
            return 0;
        }

        const Line&  line = m_Lines[line_number];
        std::int32_t col  = 0;

        for (std::int32_t i = 0; i < line.size();)
        {
            char c = line[i].m_Char;
            if (c == '\t')
            {
                col = (col / m_TabSize) * m_TabSize + m_TabSize;
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
        if (at.m_Line >= (std::int32_t)m_Lines.size() || at.m_Column == 0)
        {
            return true;
        }

        const Line&  line   = m_Lines[at.m_Line];
        std::int32_t cindex = GetCharacterIndex(at);
        if (cindex >= (std::int32_t)line.size())
        {
            return true;
        }

        if (m_ColorizerEnabled)
        {
            return line[cindex].m_ColorIndex != line[size_t(cindex - 1)].m_ColorIndex;
        }

        return dlx::IsSpace(line[cindex].m_Char) != dlx::IsSpace(line[cindex - 1].m_Char);
    }

    void CodeEditor::RemoveLine(std::int32_t start, std::int32_t end) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(end >= start);
        PHI_ASSERT(m_Lines.size() > (size_t)(end - start));

        ErrorMarkers etmp;
        for (auto& i : m_ErrorMarkers)
        {
            ErrorMarkers::value_type e(i.first >= start ? i.first - 1 : i.first, i.second);
            if (e.first >= start && e.first <= end)
            {
                continue;
            }
            etmp.insert(e);
        }
        m_ErrorMarkers = std::move(etmp);

        Breakpoints btmp;
        for (auto i : m_Breakpoints)
        {
            if (i >= start && i <= end)
            {
                continue;
            }
            btmp.insert(i >= start ? i - 1 : i);
        }
        m_Breakpoints = std::move(btmp);

        m_Lines.erase(m_Lines.begin() + start, m_Lines.begin() + end);
        PHI_ASSERT(!m_Lines.empty());

        m_TextChanged = true;
    }

    void CodeEditor::RemoveLine(std::int32_t index) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);
        PHI_ASSERT(m_Lines.size() > 1);

        ErrorMarkers etmp;
        for (auto& i : m_ErrorMarkers)
        {
            ErrorMarkers::value_type e(i.first > index ? i.first - 1 : i.first, i.second);
            if (e.first - 1 == index)
            {
                continue;
            }
            etmp.insert(e);
        }
        m_ErrorMarkers = std::move(etmp);

        Breakpoints btmp;
        for (auto i : m_Breakpoints)
        {
            if (i == index)
            {
                continue;
            }

            btmp.insert(i >= index ? i - 1 : i);
        }
        m_Breakpoints = std::move(btmp);

        m_Lines.erase(m_Lines.begin() + index);
        PHI_ASSERT(!m_Lines.empty());

        m_TextChanged = true;
    }

    CodeEditor::Line& CodeEditor::InsertLine(std::int32_t index) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);

        Line& result = *m_Lines.insert(m_Lines.begin() + index, Line());

        ErrorMarkers etmp;
        for (auto& i : m_ErrorMarkers)
        {
            etmp.insert(
                    ErrorMarkers::value_type(i.first >= index ? i.first + 1 : i.first, i.second));
        }
        m_ErrorMarkers = std::move(etmp);

        Breakpoints btmp;
        for (auto i : m_Breakpoints)
        {
            btmp.insert(i >= index ? i + 1 : i);
        }
        m_Breakpoints = std::move(btmp);

        return result;
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

        std::string r;

        std::int32_t istart = GetCharacterIndex(start);
        std::int32_t iend   = GetCharacterIndex(end);

        for (std::int32_t it = istart; it < iend; ++it)
        {
            r.push_back(m_Lines[coords.m_Line][it].m_Char);
        }

        return r;
    }

    void CodeEditor::HandleKeyboardInputs() noexcept
    {
        ImGuiIO& io    = ImGui::GetIO();
        bool     shift = io.KeyShift;
        bool     ctrl  = io.ConfigMacOSXBehaviors ? io.KeySuper : io.KeyCtrl;
        bool     alt   = io.ConfigMacOSXBehaviors ? io.KeyCtrl : io.KeyAlt;

        if (ImGui::IsWindowFocused())
        {
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
                MoveUp(1, shift);
            }
            else if (!ctrl && !alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
            {
                MoveDown(1, shift);
            }
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)))
            {
                MoveLeft(1, shift, ctrl);
            }
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)))
            {
                MoveRight(1, shift, ctrl);
            }
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp)))
            {
                MoveUp(GetPageSize() - 4, shift);
            }
            else if (!alt && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown)))
            {
                MoveDown(GetPageSize() - 4, shift);
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
                Backspace();
            }
            else if (!ctrl && !shift && !alt &&
                     ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
            {
                m_Overwrite ^= true;
            }
            else if (ctrl && !shift && !alt &&
                     ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Insert)))
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
            else if (!ctrl && shift && !alt &&
                     ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Delete)))
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
                EnterCharacter('\n', false);
            }
            else if (!IsReadOnly() && !ctrl && !alt &&
                     ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Tab)))
            {
                EnterCharacter('\t', shift);
            }

            if (!IsReadOnly() && !io.InputQueueCharacters.empty())
            {
                for (std::int32_t i = 0; i < io.InputQueueCharacters.Size; i++)
                {
                    ImWchar c = io.InputQueueCharacters[i];
                    if (c != 0 && (c == '\n' || c >= 32))
                    {
                        EnterCharacter(c, shift);
                    }
                }

                io.InputQueueCharacters.resize(0);
            }
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
        for (std::int32_t i = 0; i < (std::int32_t)PaletteIndex::Max; ++i)
        {
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(m_PaletteBase[i]);
            color.w *= ImGui::GetStyle().Alpha;
            m_Palette[i] = ImGui::ColorConvertFloat4ToU32(color);
        }

        PHI_ASSERT(m_LineBuffer.empty());

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

        std::int32_t line_no         = (std::int32_t)std::floor(scroll_y / m_CharAdvance.y);
        std::int32_t global_line_max = (std::int32_t)m_Lines.size();
        std::int32_t line_max        = std::max(
                0, std::min((std::int32_t)m_Lines.size() - 1,
                            line_no + (std::int32_t)std::floor((scroll_y + content_size.y) /
                                                               m_CharAdvance.y)));

        // Deduce m_TextStart by evaluating mLines size (global lineMax) plus two spaces as text width
        char buf[16];
        snprintf(buf, 16, " %d ", global_line_max);
        m_TextStart =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, buf, nullptr, nullptr)
                        .x +
                m_LeftMargin;

        if (!m_Lines.empty())
        {
            float space_size = ImGui::GetFont()
                                       ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ",
                                                       nullptr, nullptr)
                                       .x;

            while (line_no <= line_max)
            {
                ImVec2 line_start_screen_pos = ImVec2(
                        cursor_screen_pos.x, cursor_screen_pos.y + line_no * m_CharAdvance.y);
                ImVec2 text_screen_pos =
                        ImVec2(line_start_screen_pos.x + m_TextStart, line_start_screen_pos.y);

                Line& line             = m_Lines[line_no];
                longest                = std::max(m_TextStart + TextDistanceToLineStart(Coordinates(
                                                         line_no, GetLineMaxColumn(line_no))),
                                   longest);
                std::int32_t column_no = 0;
                Coordinates  line_start_coord(line_no, 0);
                Coordinates  line_end_coord(line_no, GetLineMaxColumn(line_no));

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

                if (sstart != -1 && ssend != -1 && sstart < ssend)
                {
                    ImVec2 vstart(line_start_screen_pos.x + m_TextStart + sstart,
                                  line_start_screen_pos.y);
                    ImVec2 vend(line_start_screen_pos.x + m_TextStart + ssend,
                                line_start_screen_pos.y + m_CharAdvance.y);

                    draw_list->AddRectFilled(vstart, vend,
                                             m_Palette[(std::int32_t)PaletteIndex::Selection]);
                }

                // Draw breakpoints
                ImVec2 start = ImVec2(line_start_screen_pos.x + scroll_x, line_start_screen_pos.y);

                if (m_Breakpoints.count(line_no + 1) != 0)
                {
                    ImVec2 end = ImVec2(line_start_screen_pos.x + content_size.x + 2.0f * scroll_x,
                                        line_start_screen_pos.y + m_CharAdvance.y);
                    draw_list->AddRectFilled(start, end,
                                             m_Palette[(std::int32_t)PaletteIndex::Breakpoint]);
                }

                // Draw error markers
                auto error_it = m_ErrorMarkers.find(line_no + 1);
                if (error_it != m_ErrorMarkers.end())
                {
                    ImVec2 end = ImVec2(line_start_screen_pos.x + content_size.x + 2.0f * scroll_x,
                                        line_start_screen_pos.y + m_CharAdvance.y);
                    draw_list->AddRectFilled(start, end,
                                             m_Palette[(std::int32_t)PaletteIndex::ErrorMarker]);

                    if (ImGui::IsMouseHoveringRect(line_start_screen_pos, end))
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                        ImGui::Text("Error at line %d:", error_it->first);
                        ImGui::PopStyleColor();
                        ImGui::Separator();
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
                        ImGui::Text("%s", error_it->second.c_str());
                        ImGui::PopStyleColor();
                        ImGui::EndTooltip();
                    }
                }

                // Draw line number (right aligned)
                snprintf(buf, 16, "%d  ", line_no + 1);

                float line_no_width = ImGui::GetFont()
                                              ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f,
                                                              buf, nullptr, nullptr)
                                              .x;
                draw_list->AddText(ImVec2(line_start_screen_pos.x + m_TextStart - line_no_width,
                                          line_start_screen_pos.y),
                                   m_Palette[(std::int32_t)PaletteIndex::LineNumber], buf);

                if (m_State.m_CursorPosition.m_Line == line_no)
                {
                    bool focused = ImGui::IsWindowFocused();

                    // Highlight the current line (where the cursor is)
                    if (!HasSelection())
                    {
                        ImVec2 end = ImVec2(start.x + content_size.x + scroll_x,
                                            start.y + m_CharAdvance.y);
                        draw_list->AddRectFilled(
                                start, end,
                                m_Palette[(std::int32_t)(
                                        focused ? PaletteIndex::CurrentLineFill :
                                                  PaletteIndex::CurrentLineFillInactive)]);
                        draw_list->AddRect(start, end,
                                           m_Palette[(std::int32_t)PaletteIndex::CurrentLineEdge],
                                           1.0f);
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
                            float        width  = 1.0f;
                            std::int32_t cindex = GetCharacterIndex(m_State.m_CursorPosition);
                            float        cx     = TextDistanceToLineStart(m_State.m_CursorPosition);

                            if (m_Overwrite && cindex < (std::int32_t)line.size())
                            {
                                char c = line[cindex].m_Char;
                                if (c == '\t')
                                {
                                    float x = (1.0f + std::floor((1.0f + cx) /
                                                                 (float(m_TabSize) * space_size))) *
                                              (float(m_TabSize) * space_size);
                                    width = x - cx;
                                }
                                else
                                {
                                    char buf2[2];
                                    buf2[0] = line[cindex].m_Char;
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
                                                     m_Palette[(std::int32_t)PaletteIndex::Cursor]);
                            if (elapsed > 800)
                            {
                                m_StartTime = time_end;
                            }
                        }
                    }
                }

                // Render colorized text
                ImU32  prev_color = line.empty() ? m_Palette[(int)PaletteIndex::Default] :
                                                   GetGlyphColor(line[0]);
                ImVec2 buffer_offset;

                for (std::int32_t i = 0; i < line.size();)
                {
                    CodeEditor::Glyph& glyph = line[i];
                    ImU32              color = GetGlyphColor(glyph);

                    if ((color != prev_color || glyph.m_Char == '\t' || glyph.m_Char == ' ') &&
                        !m_LineBuffer.empty())
                    {
                        const ImVec2 new_offset(text_screen_pos.x + buffer_offset.x,
                                                text_screen_pos.y + buffer_offset.y);
                        draw_list->AddText(new_offset, prev_color, m_LineBuffer.c_str());
                        ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(
                                ImGui::GetFontSize(), FLT_MAX, -1.0f, m_LineBuffer.c_str(), nullptr,
                                nullptr);
                        buffer_offset.x += text_size.x;
                        m_LineBuffer.clear();
                    }
                    prev_color = color;

                    if (glyph.m_Char == '\t')
                    {
                        float old_x     = buffer_offset.x;
                        buffer_offset.x = (1.0f + std::floor((1.0f + buffer_offset.x) /
                                                             (float(m_TabSize) * space_size))) *
                                          (float(m_TabSize) * space_size);
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
                        std::int32_t l = UTF8CharLength(glyph.m_Char);
                        while (l-- > 0)
                        {
                            m_LineBuffer.push_back(line[i++].m_Char);
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
        }

        ImGui::Dummy(ImVec2((longest + 2), m_Lines.size() * m_CharAdvance.y));

        if (m_ScrollToCursor)
        {
            EnsureCursorVisible();
            ImGui::SetWindowFocus();
            m_ScrollToCursor = false;
        }
    }

    void CodeEditor::Render(const ImVec2& size, bool border) noexcept
    {
        m_WithinRender          = true;
        m_CursorPositionChanged = false;

        ImGui::PushStyleColor(
                ImGuiCol_ChildBg,
                ImGui::ColorConvertU32ToFloat4(m_Palette[(std::int32_t)PaletteIndex::Background]));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

        if (ImGui::Begin("Code Editor"))
        {
            ImGui::BeginChild("Code Editor", size, border,
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
                    AddErrorMarker(err.line_number, err.message);
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

        for (char chr : text)
        {
            if (chr == '\r' || std::iscntrl(chr))
            {
                // ignore the carriage return character and control characters
            }
            else if (chr == '\n')
            {
                m_Lines.emplace_back(Line());
            }
            else
            {
                m_Lines.back().emplace_back(Glyph(chr, PaletteIndex::Default));
            }
        }

        m_TextChanged = true;
        m_ScrollToTop = true;

        m_UndoBuffer.clear();
        m_UndoIndex = 0;

        Colorize();
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

            for (size_t i = 0; i < lines.size(); ++i)
            {
                const std::string& line = lines[i];

                m_Lines[i].reserve(line.size());
                for (char j : line)
                {
                    PHI_ASSERT(j != '\n');
                    m_Lines[i].emplace_back(Glyph(j, PaletteIndex::Default));
                }
            }
        }

        m_TextChanged = true;
        m_ScrollToTop = true;

        m_UndoBuffer.clear();
        m_UndoIndex = 0;

        Colorize();
    }

    void CodeEditor::EnterCharacter(ImWchar character, bool shift) noexcept
    {
        PHI_ASSERT(!m_ReadOnly);

        UndoRecord u;

        u.m_Before = m_State;

        if (HasSelection())
        {
            if (character == '\t' &&
                m_State.m_SelectionStart.m_Line != m_State.m_SelectionEnd.m_Line)
            {
                Coordinates start        = m_State.m_SelectionStart;
                Coordinates end          = m_State.m_SelectionEnd;
                Coordinates original_end = end;

                if (start > end)
                {
                    std::swap(start, end);
                }
                start.m_Column = 0;
                //			end.mColumn = end.mLine < mLines.size() ? mLines[end.mLine].size() : 0;
                if (end.m_Column == 0 && end.m_Line > 0)
                {
                    --end.m_Line;
                }
                if (end.m_Line >= (int32_t)m_Lines.size())
                {
                    end.m_Line = m_Lines.empty() ? 0 : (int32_t)m_Lines.size() - 1;
                }
                end.m_Column = GetLineMaxColumn(end.m_Line);

                //if (end.mColumn >= GetLineMaxColumn(end.mLine))
                //	end.mColumn = GetLineMaxColumn(end.mLine) - 1;

                u.m_RemovedStart = start;
                u.m_RemovedEnd   = end;
                u.m_Removed      = GetText(start, end);

                bool modified = false;

                for (int32_t i = start.m_Line; i <= end.m_Line; i++)
                {
                    Line& line = m_Lines[i];
                    if (shift)
                    {
                        if (!line.empty())
                        {
                            if (line.front().m_Char == '\t')
                            {
                                line.erase(line.begin());
                                modified = true;
                            }
                            else
                            {
                                for (int32_t j = 0;
                                     j < m_TabSize && !line.empty() && line.front().m_Char == ' ';
                                     j++)
                                {
                                    line.erase(line.begin());
                                    modified = true;
                                }
                            }
                        }
                    }
                    else
                    {
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

                    u.m_AddedStart = start;
                    u.m_AddedEnd   = range_end;
                    u.m_After      = m_State;

                    m_State.m_SelectionStart = start;
                    m_State.m_SelectionEnd   = end;
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

        PHI_ASSERT(!m_Lines.empty());

        if (character == '\n')
        {
            InsertLine(coord.m_Line + 1);
            Line& line     = m_Lines[coord.m_Line];
            Line& new_line = m_Lines[coord.m_Line + 1];

            for (std::size_t it = 0;
                 it < line.size() && isascii(line[it].m_Char) && dlx::IsBlank(line[it].m_Char);
                 ++it)
            {
                new_line.push_back(line[it]);
            }

            const std::size_t whitespace_size = new_line.size();
            std::int32_t      cindex          = GetCharacterIndex(coord);
            new_line.insert(new_line.end(), line.begin() + cindex, line.end());
            line.erase(line.begin() + cindex, line.begin() + line.size());
            SetCursorPosition(Coordinates(
                    coord.m_Line + 1, GetCharacterColumn(coord.m_Line + 1, (int)whitespace_size)));
            u.m_Added = (char)character;
        }
        else
        {
            char         buf[7];
            std::int32_t e = ImTextCharToUtf8(buf, 7, character);
            if (e > 0)
            {
                buf[e]         = '\0';
                Line&   line   = m_Lines[coord.m_Line];
                int32_t cindex = GetCharacterIndex(coord);

                if (m_Overwrite && cindex < (int)line.size())
                {
                    std::int32_t d = UTF8CharLength(line[cindex].m_Char);

                    u.m_RemovedStart = m_State.m_CursorPosition;
                    u.m_RemovedEnd =
                            Coordinates(coord.m_Line, GetCharacterColumn(coord.m_Line, cindex + d));

                    while (d-- > 0 && cindex < (int)line.size())
                    {
                        u.m_Removed += line[cindex].m_Char;
                        line.erase(line.begin() + cindex);
                    }
                }

                for (char* p = buf; *p != '\0'; p++, ++cindex)
                {
                    line.insert(line.begin() + cindex, Glyph(*p, PaletteIndex::Default));
                }
                u.m_Added = buf;

                SetCursorPosition(
                        Coordinates(coord.m_Line, GetCharacterColumn(coord.m_Line, cindex)));
            }
            else
            {
                return;
            }
        }

        m_TextChanged = true;

        u.m_AddedEnd = GetActualCursorCoordinates();
        u.m_After    = m_State;

        AddUndo(u);

        Colorize(coord.m_Line - 1, 3);
        EnsureCursorVisible();
    }

    void CodeEditor::SetReadOnly(bool value) noexcept
    {
        m_ReadOnly = value;
    }

    void CodeEditor::SetColorizerEnable(bool value) noexcept
    {
        m_ColorizerEnabled = value;
    }

    void CodeEditor::SetCursorPosition(const Coordinates& position) noexcept
    {
        // Sanitize
        Coordinates new_pos = SanitizeCoordinates(position);

        if (m_State.m_CursorPosition != new_pos)
        {
            m_State.m_CursorPosition = new_pos;
            m_CursorPositionChanged  = true;
            EnsureCursorVisible();
        }
    }

    void CodeEditor::SetSelectionStart(const Coordinates& position) noexcept
    {
        m_State.m_SelectionStart = SanitizeCoordinates(position);
        if (m_State.m_SelectionStart > m_State.m_SelectionEnd)
        {
            std::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
        }
    }

    void CodeEditor::SetSelectionEnd(const Coordinates& position) noexcept
    {
        m_State.m_SelectionEnd = SanitizeCoordinates(position);
        if (m_State.m_SelectionStart > m_State.m_SelectionEnd)
        {
            std::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
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
            std::swap(m_State.m_SelectionStart, m_State.m_SelectionEnd);
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
                const std::int32_t line_no = m_State.m_SelectionEnd.m_Line;
                const std::size_t  line_size =
                        (std::size_t)line_no < m_Lines.size() ? m_Lines[line_no].size() : 0;
                m_State.m_SelectionStart = Coordinates(m_State.m_SelectionStart.m_Line, 0);
                m_State.m_SelectionEnd   = Coordinates(line_no, GetLineMaxColumn(line_no));
                break;
            }
            default:
                break;
        }

        if (m_State.m_SelectionStart != old_sel_start || m_State.m_SelectionEnd != old_sel_end)
        {
            m_CursorPositionChanged = true;
        }
    }

    void CodeEditor::SetTabSize(std::uint32_t value) noexcept
    {
        m_TabSize = std::clamp(value, 1u, 32u);
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

        Coordinates  pos         = GetActualCursorCoordinates();
        Coordinates  start       = std::min(pos, m_State.m_SelectionStart);
        std::int32_t total_lines = pos.m_Line - start.m_Line;

        total_lines += InsertTextAt(pos, value);

        SetSelection(pos, pos);
        SetCursorPosition(pos);
        Colorize(start.m_Line - 1, total_lines + 2);
    }

    void CodeEditor::DeleteSelection() noexcept
    {
        PHI_ASSERT(m_State.m_SelectionEnd >= m_State.m_SelectionStart);
        PHI_ASSERT(m_State.m_SelectionStart.m_Line < m_Lines.size());
        PHI_ASSERT(m_State.m_SelectionEnd.m_Line < m_Lines.size());

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

    void CodeEditor::MoveUp(std::uint32_t amount, bool select) noexcept
    {
        Coordinates old_pos = m_State.m_CursorPosition;
        m_State.m_CursorPosition.m_Line =
                std::max(0, static_cast<std::int32_t>(m_State.m_CursorPosition.m_Line - amount));

        if (old_pos != m_State.m_CursorPosition)
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
    }

    void CodeEditor::MoveDown(std::uint32_t amount, bool select) noexcept
    {
        PHI_ASSERT(m_State.m_CursorPosition.m_Column >= 0);

        Coordinates old_pos = m_State.m_CursorPosition;
        m_State.m_CursorPosition.m_Line =
                std::max(0u, std::min(static_cast<std::int32_t>(m_Lines.size()) - 1u,
                                      m_State.m_CursorPosition.m_Line + amount));

        if (m_State.m_CursorPosition != old_pos)
        {
            if (select)
            {
                if (old_pos == m_InteractiveEnd)
                {
                    m_InteractiveEnd = m_State.m_CursorPosition;
                }
                else if (old_pos == m_InteractiveStart)
                {
                    m_InteractiveStart = m_State.m_CursorPosition;
                }
                else
                {
                    m_InteractiveStart = old_pos;
                    m_InteractiveEnd   = m_State.m_CursorPosition;
                }
            }
            else
            {
                m_InteractiveStart = m_InteractiveEnd = m_State.m_CursorPosition;
            }

            SetSelection(m_InteractiveStart, m_InteractiveEnd);

            EnsureCursorVisible();
        }
    }

    static bool IsUTFSequence(char c) noexcept
    {
        return (c & 0xC0) == 0x80;
    }

    void CodeEditor::MoveLeft(std::uint32_t amount, bool select, bool word_mode) noexcept
    {
        if (m_Lines.empty())
        {
            return;
        }

        Coordinates old_pos      = m_State.m_CursorPosition;
        m_State.m_CursorPosition = GetActualCursorCoordinates();
        std::int32_t line        = m_State.m_CursorPosition.m_Line;
        std::int32_t cindex      = GetCharacterIndex(m_State.m_CursorPosition);

        amount = std::min(amount, static_cast<std::uint32_t>(m_Lines[line].size()));

        while (amount-- > 0)
        {
            if (cindex == 0)
            {
                if (line > 0)
                {
                    --line;
                    if ((std::int32_t)m_Lines.size() > line)
                    {
                        cindex = (std::int32_t)m_Lines[line].size();
                    }
                    else
                    {
                        cindex = 0;
                    }
                }
            }
            else
            {
                --cindex;
                if (cindex > 0)
                {
                    if ((std::int32_t)m_Lines.size() > line)
                    {
                        while (cindex > 0 && IsUTFSequence(m_Lines[line][cindex].m_Char))
                        {
                            --cindex;
                        }
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

        PHI_ASSERT(m_State.m_CursorPosition.m_Column >= 0);
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

        SetSelection(m_InteractiveStart, m_InteractiveEnd,
                     select && word_mode ? SelectionMode::Word : SelectionMode::Normal);

        EnsureCursorVisible();
    }

    void CodeEditor::MoveRight(std::uint32_t amount, bool select, bool word_mode) noexcept
    {
        Coordinates old_pos = m_State.m_CursorPosition;

        if (m_Lines.empty() || old_pos.m_Line >= m_Lines.size())
        {
            return;
        }

        std::int32_t cindex = GetCharacterIndex(m_State.m_CursorPosition);
        while (amount-- > 0)
        {
            std::int32_t lindex = m_State.m_CursorPosition.m_Line;
            Line&        line   = m_Lines[lindex];

            if (cindex >= line.size())
            {
                if (m_State.m_CursorPosition.m_Line < m_Lines.size() - 1)
                {
                    m_State.m_CursorPosition.m_Line =
                            std::max(0, std::min((std::int32_t)m_Lines.size() - 1,
                                                 m_State.m_CursorPosition.m_Line + 1));
                    m_State.m_CursorPosition.m_Column = 0;
                }
                else
                {
                    return;
                }
            }
            else
            {
                cindex += UTF8CharLength(line[cindex].m_Char);
                m_State.m_CursorPosition = Coordinates(lindex, GetCharacterColumn(lindex, cindex));

                if (word_mode)
                {
                    m_State.m_CursorPosition = FindNextWord(m_State.m_CursorPosition);
                }
            }
        }

        if (select)
        {
            if (old_pos == m_InteractiveEnd)
            {
                m_InteractiveEnd = SanitizeCoordinates(m_State.m_CursorPosition);
            }
            else if (old_pos == m_InteractiveStart)
            {
                m_InteractiveStart = m_State.m_CursorPosition;
            }
            else
            {
                m_InteractiveStart = old_pos;
                m_InteractiveEnd   = m_State.m_CursorPosition;
            }
        }
        else
        {
            m_InteractiveStart = m_InteractiveEnd = m_State.m_CursorPosition;
        }

        SetSelection(m_InteractiveStart, m_InteractiveEnd,
                     select && word_mode ? SelectionMode::Word : SelectionMode::Normal);

        EnsureCursorVisible();
    }

    void CodeEditor::MoveTop(bool select) noexcept
    {
        Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(Coordinates(0, 0));

        if (m_State.m_CursorPosition != old_pos)
        {
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
        }
    }

    void CodeEditor::CodeEditor::MoveBottom(bool select) noexcept
    {
        Coordinates old_pos = GetCursorPosition();
        Coordinates new_pos = Coordinates((std::int32_t)m_Lines.size() - 1, 0);
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
    }

    void CodeEditor::MoveHome(bool select) noexcept
    {
        Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(Coordinates(m_State.m_CursorPosition.m_Line, 0));

        if (m_State.m_CursorPosition != old_pos)
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
        }
    }

    void CodeEditor::MoveEnd(bool select) noexcept
    {
        Coordinates old_pos = m_State.m_CursorPosition;
        SetCursorPosition(
                Coordinates(m_State.m_CursorPosition.m_Line, GetLineMaxColumn(old_pos.m_Line)));

        if (m_State.m_CursorPosition != old_pos)
        {
            if (select)
            {
                if (old_pos == m_InteractiveEnd)
                {
                    m_InteractiveEnd = m_State.m_CursorPosition;
                }
                else if (old_pos == m_InteractiveStart)
                {
                    m_InteractiveStart = m_State.m_CursorPosition;
                }
                else
                {
                    m_InteractiveStart = old_pos;
                    m_InteractiveEnd   = m_State.m_CursorPosition;
                }
            }
            else
            {
                m_InteractiveStart = m_InteractiveEnd = m_State.m_CursorPosition;
            }

            SetSelection(m_InteractiveStart, m_InteractiveEnd);
        }
    }

    void CodeEditor::Delete() noexcept
    {
        if (m_Lines.empty() || m_ReadOnly)
        {
            return;
        }

        UndoRecord u;
        u.m_Before = m_State;

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
            Line& line = m_Lines[pos.m_Line];

            if (pos.m_Column == GetLineMaxColumn(pos.m_Line))
            {
                if (pos.m_Line == (std::int32_t)m_Lines.size() - 1)
                {
                    return;
                }

                u.m_Removed      = '\n';
                u.m_RemovedStart = u.m_RemovedEnd = GetActualCursorCoordinates();
                Advance(u.m_RemovedEnd);

                Line& next_line = m_Lines[pos.m_Line + 1];
                line.insert(line.end(), next_line.begin(), next_line.end());

                PHI_ASSERT(pos.m_Line <= m_Lines.size());
                RemoveLine(pos.m_Line + 1);
            }
            else
            {
                // Nothing todo when line is empty
                if (line.empty())
                {
                    return;
                }

                std::int32_t cindex = GetCharacterIndex(pos);
                // TODO: Bit of a hack to get around invalid acceses the assert below should always be true
                if (cindex >= line.size())
                {
                    return;
                }
                PHI_ASSERT(cindex < line.size());

                u.m_RemovedStart = u.m_RemovedEnd = GetActualCursorCoordinates();
                u.m_RemovedEnd.m_Column++;
                u.m_Removed = GetText(u.m_RemovedStart, u.m_RemovedEnd);

                std::int32_t d = UTF8CharLength(line[cindex].m_Char);
                while (d-- > 0 && cindex < (std::int32_t)line.size())
                {
                    line.erase(line.begin() + cindex);
                }
            }

            m_TextChanged = true;

            Colorize(pos.m_Line, 1);
        }

        u.m_After = m_State;
        AddUndo(u);
    }

    void CodeEditor::Backspace() noexcept
    {
        PHI_ASSERT(!m_ReadOnly);

        if (m_Lines.empty())
        {
            return;
        }

        UndoRecord u;
        u.m_Before = m_State;

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

                Line&        line      = m_Lines[m_State.m_CursorPosition.m_Line];
                Line&        prev_line = m_Lines[m_State.m_CursorPosition.m_Line - 1];
                std::int32_t prev_size = GetLineMaxColumn(m_State.m_CursorPosition.m_Line - 1);
                prev_line.insert(prev_line.end(), line.begin(), line.end());

                ErrorMarkers etmp;
                for (auto& i : m_ErrorMarkers)
                {
                    etmp.insert(ErrorMarkers::value_type(
                            i.first - 1 == m_State.m_CursorPosition.m_Line ? i.first - 1 : i.first,
                            i.second));
                }
                m_ErrorMarkers = std::move(etmp);

                RemoveLine(m_State.m_CursorPosition.m_Line);
                --m_State.m_CursorPosition.m_Line;
                m_State.m_CursorPosition.m_Column = prev_size;
            }
            else
            {
                Line&        line   = m_Lines[m_State.m_CursorPosition.m_Line];
                std::int32_t cindex = GetCharacterIndex(pos) - 1;
                std::int32_t cend   = cindex + 1;
                while (cindex > 0 && IsUTFSequence(line[cindex].m_Char))
                {
                    --cindex;
                }

                //if (cindex > 0 && UTF8CharLength(line[cindex]) > 1)
                //	--cindex;

                u.m_RemovedStart = u.m_RemovedEnd = GetActualCursorCoordinates();
                --u.m_RemovedStart.m_Column;
                --m_State.m_CursorPosition.m_Column;

                while (cindex < line.size() && cend-- > cindex)
                {
                    u.m_Removed += line[cindex].m_Char;
                    line.erase(line.begin() + cindex);
                }
            }

            m_TextChanged = true;

            EnsureCursorVisible();
            Colorize(m_State.m_CursorPosition.m_Line, 1);
        }

        u.m_After = m_State;
        AddUndo(u);
    }

    void CodeEditor::SelectWordUnderCursor() noexcept
    {
        Coordinates c = GetCursorPosition();
        SetSelection(FindWordStart(c), FindWordEnd(c));
    }

    void CodeEditor::SelectAll() noexcept
    {
        SetSelection(Coordinates(0, 0), Coordinates((std::int32_t)m_Lines.size(), 0));
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

    void CodeEditor::Copy() noexcept
    {
        if (HasSelection())
        {
            ImGui::SetClipboardText(GetSelectedText().c_str());
        }
        else
        {
            if (!m_Lines.empty())
            {
                std::string str;
                Line&       line = m_Lines[GetActualCursorCoordinates().m_Line];

                for (Glyph& g : line)
                {
                    str.push_back(g.m_Char);
                }

                ImGui::SetClipboardText(str.c_str());
            }
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
                u.m_Before       = m_State;
                u.m_Removed      = GetSelectedText();
                u.m_RemovedStart = m_State.m_SelectionStart;
                u.m_RemovedEnd   = m_State.m_SelectionEnd;

                Copy();
                DeleteSelection();

                u.m_After = m_State;
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
        if (clip_text != nullptr && strlen(clip_text) > 0)
        {
            UndoRecord u;
            u.m_Before = m_State;

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
    }

    bool CodeEditor::CanUndo() const noexcept
    {
        return !m_ReadOnly && m_UndoIndex > 0;
    }

    bool CodeEditor::CanRedo() const noexcept
    {
        return !m_ReadOnly && m_UndoIndex < m_UndoBuffer.size();
    }

    void CodeEditor::Undo(std::uint32_t steps) noexcept
    {
        while (CanUndo() && steps-- > 0)
        {
            m_UndoBuffer[--m_UndoIndex].Undo(this);
        }
    }

    void CodeEditor::Redo(std::uint32_t steps) noexcept
    {
        while (CanRedo() && steps-- > 0)
        {
            m_UndoBuffer[m_UndoIndex++].Redo(this);
        }
    }

    const CodeEditor::Palette& CodeEditor::GetDarkPalette() noexcept
    {
        const static Palette p = {{
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
        const static Palette p = {{
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
        const static Palette p = {{
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

    std::string CodeEditor::GetText() const noexcept
    {
        return GetText(Coordinates(), Coordinates((std::int32_t)m_Lines.size(), 0));
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

            result.emplace_back(std::move(text));
        }

        return result;
    }

    std::string CodeEditor::GetSelectedText() const noexcept
    {
        return GetText(m_State.m_SelectionStart, m_State.m_SelectionEnd);
    }

    std::string CodeEditor::GetCurrentLineText() const noexcept
    {
        std::int32_t line_length = GetLineMaxColumn(m_State.m_CursorPosition.m_Line);

        return GetText(Coordinates(m_State.m_CursorPosition.m_Line, 0),
                       Coordinates(m_State.m_CursorPosition.m_Line, line_length));
    }

    void CodeEditor::Colorize(std::int32_t from_line, std::int32_t count) noexcept
    {
        std::int32_t to_line = count == -1 ?
                                       (std::int32_t)m_Lines.size() :
                                       std::min((std::int32_t)m_Lines.size(), from_line + count);

        m_ColorRangeMin = std::min(m_ColorRangeMin, from_line);
        m_ColorRangeMax = std::max(m_ColorRangeMax, to_line);
        m_ColorRangeMin = std::max(0, m_ColorRangeMin);
        m_ColorRangeMax = std::max(m_ColorRangeMin, m_ColorRangeMax);
    }

    float CodeEditor::TextDistanceToLineStart(const Coordinates& from) const noexcept
    {
        const Line& line     = m_Lines[from.m_Line];
        float       distance = 0.0f;
        float       space_size =
                ImGui::GetFont()
                        ->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr)
                        .x;
        std::int32_t col_index = GetCharacterIndex(from);
        for (size_t it = 0u; it < line.size() && it < col_index;)
        {
            if (line[it].m_Char == '\t')
            {
                distance =
                        (1.0f + std::floor((1.0f + distance) / (float(m_TabSize) * space_size))) *
                        (float(m_TabSize) * space_size);
                ++it;
            }
            else
            {
                std::int32_t d = UTF8CharLength(line[it].m_Char);
                char         temp_c_string[7];
                std::int32_t i = 0;
                for (; i < 6 && d-- > 0 && it < (std::int32_t)line.size(); i++, it++)
                {
                    temp_c_string[i] = line[it].m_Char;
                }

                temp_c_string[i] = '\0';
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

        std::int32_t top    = 1 + (std::int32_t)std::ceil(scroll_y / m_CharAdvance.y);
        std::int32_t bottom = (std::int32_t)std::ceil((scroll_y + height) / m_CharAdvance.y);

        std::int32_t left  = (std::int32_t)std::ceil(scroll_x / m_CharAdvance.x);
        std::int32_t right = (std::int32_t)std::ceil((scroll_x + width) / m_CharAdvance.x);

        Coordinates pos = GetActualCursorCoordinates();
        float       len = TextDistanceToLineStart(pos);

        if (pos.m_Line < top)
        {
            ImGui::SetScrollY(std::max(0.0f, (pos.m_Line - 1) * m_CharAdvance.y));
        }
        if (pos.m_Line > bottom - 4)
        {
            ImGui::SetScrollY(std::max(0.0f, (pos.m_Line + 4) * m_CharAdvance.y - height));
        }
        if (len + m_TextStart < left + 4)
        {
            ImGui::SetScrollX(std::max(0.0f, len + m_TextStart - 4));
        }
        if (len + m_TextStart > right - 4)
        {
            ImGui::SetScrollX(std::max(0.0f, len + m_TextStart + 4 - width));
        }
    }

    std::int32_t CodeEditor::GetPageSize() const noexcept
    {
        float height = ImGui::GetWindowHeight() - 20.0f;
        return (std::int32_t)std::floor(height / m_CharAdvance.y);
    }

    CodeEditor::UndoRecord::UndoRecord(
            const std::string& added, const CodeEditor::Coordinates added_start,
            const CodeEditor::Coordinates added_end, const std::string& removed,
            const CodeEditor::Coordinates removed_start, const CodeEditor::Coordinates removed_end,
            CodeEditor::EditorState& before, CodeEditor::EditorState& after) noexcept
        : m_Added(added)
        , m_AddedStart(added_start)
        , m_AddedEnd(added_end)
        , m_Removed(removed)
        , m_RemovedStart(removed_start)
        , m_RemovedEnd(removed_end)
        , m_Before(before)
        , m_After(after)
    {
        PHI_ASSERT(m_AddedStart <= m_AddedEnd);
        PHI_ASSERT(m_RemovedStart <= m_RemovedEnd);
    }

    void CodeEditor::UndoRecord::Undo(CodeEditor* editor) noexcept
    {
        PHI_ASSERT(editor != nullptr);

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

        editor->m_State = m_Before;
        editor->EnsureCursorVisible();
    }

    void CodeEditor::UndoRecord::Redo(CodeEditor* editor) noexcept
    {
        PHI_ASSERT(editor != nullptr);

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

        editor->m_State = m_After;
        editor->EnsureCursorVisible();
    }

    CodeEditor::Coordinates::Coordinates() noexcept
        : m_Line(0)
        , m_Column(0)
    {}

    CodeEditor::Coordinates::Coordinates(dont_use, dont_use) noexcept
        : m_Line{-1}
        , m_Column{-1}
    {}

    CodeEditor::Coordinates::Coordinates(std::int32_t line, std::int32_t column) noexcept
        : m_Line(line)
        , m_Column(column)
    {
        PHI_ASSERT(line >= 0);
        PHI_ASSERT(column >= 0);
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

    const CodeEditor::Palette& CodeEditor::GetPalette() const noexcept
    {
        return m_PaletteBase;
    }

    void CodeEditor::SetErrorMarkers(const ErrorMarkers& markers) noexcept
    {
        m_ErrorMarkers = markers;
    }

    void CodeEditor::AddErrorMarker(const std::uint32_t line_number,
                                    const std::string&  message) noexcept
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

    std::uint32_t CodeEditor::GetTotalLines() const noexcept
    {
        return (std::uint32_t)m_Lines.size();
    }

    void CodeEditor::SetOverwrite(bool overwrite) noexcept
    {
        m_Overwrite = overwrite;
    }

    bool CodeEditor::IsOverwrite() const noexcept
    {
        return m_Overwrite;
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

    bool CodeEditor::IsColorizerEnabled() const noexcept
    {
        return m_ColorizerEnabled;
    }

    CodeEditor::Coordinates CodeEditor::GetCursorPosition() const noexcept
    {
        return GetActualCursorCoordinates();
    }

    void CodeEditor::SetShowWhitespaces(bool value) noexcept
    {
        m_ShowWhitespaces = value;
    }

    bool CodeEditor::IsShowingWhitespaces() const noexcept
    {
        return m_ShowWhitespaces;
    }

    std::uint32_t CodeEditor::GetTabSize() const noexcept
    {
        return m_TabSize;
    }

    ImU32 CodeEditor::GetGlyphColor(const Glyph& glyph) const noexcept
    {
        if (!m_ColorizerEnabled)
        {
            return m_Palette[(int)PaletteIndex::Default];
        }

        return m_Palette[(int)glyph.m_ColorIndex];
    }

    void CodeEditor::ColorizeToken(const dlx::Token& token) noexcept
    {
        //PHI_LOG_DEBUG("Colorizing token: {}", token.DebugInfo());

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

        //PHI_LOG_DEBUG("palette_index: {}", magic_enum::enum_name(palette_index));
        //PHI_LOG_DEBUG("token length: {}", token.GetLength().get());

        Line& line = m_Lines[(token.GetLineNumber() - 1u).get()];
        //PHI_LOG_DEBUG("Line number: {}", (token.GetLineNumber() - 1u).get());
        //PHI_LOG_DEBUG("line empty: {}", line.empty());

        for (std::size_t index{static_cast<std::size_t>((token.GetColumn() - 1u).get())};
             index < token.GetColumn() + token.GetLength() - 1u; ++index)
        {
            //PHI_LOG_DEBUG("index:{:d}, max size: {:d}", index, line.size());
            PHI_DBG_ASSERT(index < line.size());
            line[index].m_ColorIndex = palette_index;
        }

        //PHI_LOG_DEBUG("Finished colorizing");
    }

    void CodeEditor::ColorizeInternal() noexcept
    {
        const dlx::ParsedProgram& program = m_Emulator->GetProgram();

        //PHI_LOG_DEBUG("Tokens: ");
        for (const dlx::Token& token : program.m_Tokens)
        {
            ColorizeToken(token);
        }
        //PHI_LOG_DEBUG("End of Tokens\n");
    }

    void CodeEditor::ResetState() noexcept
    {
        m_State.m_CursorPosition = Coordinates(0, 0);
        m_State.m_SelectionStart = Coordinates(0, 0);
        m_State.m_SelectionEnd   = Coordinates(0, 0);
    }

    CodeEditor::Glyph::Glyph(Char character, PaletteIndex color_index) noexcept
        : m_Char(character)
        , m_ColorIndex(color_index)
    {}

    std::string CodeEditor::GetEditorDump() const noexcept
    {
        std::string str;

        return str;
    }
} // namespace dlxemu
