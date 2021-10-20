// This file is heavily inspired by BalazsJako ImGuiColorTextEdit library https://github.com/BalazsJako/ImGuiColorTextEdit
// licensed under the MIT license https://github.com/BalazsJako/ImGuiColorTextEdit/blob/master/LICENSE
// Original file at https://github.com/BalazsJako/ImGuiColorTextEdit/blob/master/TextEditor.h
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

#pragma once

#include <DLX/OpCode.hpp>
#include <DLX/Token.hpp>
#include <Phi/Core/Types.hpp>
#include <imgui.h>
#include <array>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dlxemu
{
    class Emulator;

    class CodeEditor
    {
    public:
        enum class PaletteIndex
        {
            Default,
            OpCode,
            Register,
            IntegerLiteral,
            Comment,
            Background,
            Cursor,
            Selection,
            ErrorMarker,
            Breakpoint,
            LineNumber,
            CurrentLineFill,
            CurrentLineFillInactive,
            CurrentLineEdge,
            Max
        };

        enum class SelectionMode
        {
            Normal,
            Word,
            Line
        };

        class Breakpoint
        {
        public:
            std::int32_t m_Line{-1};
            bool         m_Enabled{false};
            std::string  m_Condition;
        };

        // Represents a character coordinate from the user's point of view,
        // i. e. consider an uniform grid (assuming fixed-width font) on the
        // screen as it is rendered, and each cell has its own coordinate, starting from 0.
        // Tabs are counted as [1..mTabSize] count empty spaces, depending on
        // how many space is necessary to reach the next tab stop.
        // For example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when mTabSize = 4,
        // because it is rendered as "    ABC" on the screen.
        class Coordinates
        {
        public:
            Coordinates() noexcept;

            Coordinates(std::int32_t line, std::int32_t column) noexcept;

            [[nodiscard]] static Coordinates Invalid() noexcept;

            bool operator==(const Coordinates& o) const noexcept;

            bool operator!=(const Coordinates& o) const noexcept;

            bool operator<(const Coordinates& o) const noexcept;

            bool operator>(const Coordinates& o) const noexcept;

            bool operator<=(const Coordinates& o) const noexcept;

            bool operator>=(const Coordinates& o) const noexcept;

            std::int32_t m_Line;
            std::int32_t m_Column;

        private:
            struct dont_use
            {};

            Coordinates(dont_use, dont_use) noexcept;
        };

        using ErrorMarkers = std::map<std::uint32_t, std::string>;
        using Breakpoints  = std::unordered_set<std::uint32_t>;
        using Palette      = std::array<ImU32, static_cast<std::size_t>(PaletteIndex::Max)>;
        using Char         = uint8_t;

        class Glyph
        {
        public:
            Glyph(Char character, PaletteIndex color_index) noexcept;

            Char         m_Char;
            PaletteIndex m_ColorIndex = PaletteIndex::Default;
        };

        using Line  = std::vector<Glyph>;
        using Lines = std::vector<Line>;

        explicit CodeEditor(Emulator* emulator) noexcept;

        [[nodiscard]] const Palette& GetPalette() const noexcept;
        void                         SetPalette(const Palette& value) noexcept;

        void SetErrorMarkers(const ErrorMarkers& markers) noexcept;

        void AddErrorMarker(const std::uint32_t line_number, const std::string& message) noexcept;

        void ClearErrorMarkers() noexcept;

        void SetBreakpoints(const Breakpoints& markers) noexcept;

        void Render(const ImVec2& size = ImVec2(), bool border = false) noexcept;
        void SetText(const std::string& text) noexcept;
        [[nodiscard]] std::string GetText() const noexcept;

        void SetTextLines(const std::vector<std::string>& lines) noexcept;
        [[nodiscard]] std::vector<std::string> GetTextLines() const noexcept;

        [[nodiscard]] std::string GetSelectedText() const noexcept;
        [[nodiscard]] std::string GetCurrentLineText() const noexcept;

        std::uint32_t      GetTotalLines() const noexcept;
        [[nodiscard]] bool IsOverwrite() const noexcept;

        void               SetReadOnly(bool value) noexcept;
        [[nodiscard]] bool IsReadOnly() const noexcept;
        [[nodiscard]] bool IsTextChanged() const noexcept;
        [[nodiscard]] bool IsCursorPositionChanged() const noexcept;

        [[nodiscard]] bool IsColorizerEnabled() const noexcept;
        void               SetColorizerEnable(bool value) noexcept;

        [[nodiscard]] Coordinates GetCursorPosition() const noexcept;
        void                      SetCursorPosition(const Coordinates& position) noexcept;

        void               SetShowWhitespaces(bool value) noexcept;
        [[nodiscard]] bool IsShowingWhitespaces() const noexcept;

        void                        SetTabSize(std::uint32_t value) noexcept;
        [[nodiscard]] std::uint32_t GetTabSize() const noexcept;

        void InsertText(const std::string& value) noexcept;
        void InsertText(const char* value) noexcept;

        void MoveUp(std::uint32_t amount = 1, bool select = false) noexcept;
        void MoveDown(std::uint32_t amount = 1, bool select = false) noexcept;
        void MoveLeft(std::uint32_t amount = 1, bool select = false,
                      bool word_mode = false) noexcept;
        void MoveRight(std::uint32_t amount = 1, bool select = false,
                       bool word_mode = false) noexcept;
        void MoveTop(bool select = false) noexcept;
        void MoveBottom(bool select = false) noexcept;
        void MoveHome(bool select = false) noexcept;
        void MoveEnd(bool select = false) noexcept;

        void                      SetSelectionStart(const Coordinates& position) noexcept;
        void                      SetSelectionEnd(const Coordinates& position) noexcept;
        void                      SetSelection(const Coordinates& start, const Coordinates& end,
                                               SelectionMode mode = SelectionMode::Normal) noexcept;
        void                      SelectWordUnderCursor() noexcept;
        void                      SelectAll() noexcept;
        [[nodiscard]] bool        HasSelection() const noexcept;
        [[nodiscard]] Coordinates GetSelectionStart() const noexcept;
        [[nodiscard]] Coordinates GetSelectionEnd() const noexcept;

        void Copy() noexcept;
        void Cut() noexcept;
        void Paste() noexcept;
        void Delete() noexcept;

        [[nodiscard]] bool CanUndo() const noexcept;
        [[nodiscard]] bool CanRedo() const noexcept;
        void               Undo(std::uint32_t steps = 1) noexcept;
        void               Redo(std::uint32_t steps = 1) noexcept;

        [[nodiscard]] std::string GetEditorDump() const noexcept;

        static const Palette& GetDarkPalette() noexcept;
        static const Palette& GetLightPalette() noexcept;
        static const Palette& GetRetroBluePalette() noexcept;

    private:
        class EditorState
        {
        public:
            Coordinates m_SelectionStart;
            Coordinates m_SelectionEnd;
            Coordinates m_CursorPosition;
        };

        class UndoRecord
        {
        public:
            UndoRecord() = default;

            UndoRecord(const std::string& added, const CodeEditor::Coordinates added_start,
                       const CodeEditor::Coordinates added_end,

                       const std::string& removed, const CodeEditor::Coordinates removed_start,
                       const CodeEditor::Coordinates removed_end,

                       CodeEditor::EditorState& before, CodeEditor::EditorState& after) noexcept;

            void Undo(CodeEditor* editor) noexcept;
            void Redo(CodeEditor* editor) noexcept;

            std::string m_Added;
            Coordinates m_AddedStart;
            Coordinates m_AddedEnd;

            std::string m_Removed;
            Coordinates m_RemovedStart;
            Coordinates m_RemovedEnd;

            EditorState m_Before;
            EditorState m_After;
        };

        using UndoBuffer = std::vector<UndoRecord>;

        void                Colorize(std::int32_t from_line = 0, std::int32_t count = -1) noexcept;
        [[nodiscard]] float TextDistanceToLineStart(const Coordinates& from) const noexcept;
        void                EnsureCursorVisible() noexcept;
        [[nodiscard]] std::int32_t GetPageSize() const noexcept;
        [[nodiscard]] std::string  GetText(const Coordinates& start,
                                           const Coordinates& end) const noexcept;
        [[nodiscard]] Coordinates  GetActualCursorCoordinates() const noexcept;
        [[nodiscard]] Coordinates  SanitizeCoordinates(const Coordinates& value) const noexcept;
        void                       Advance(Coordinates& coordinates) const noexcept;
        void         DeleteRange(const Coordinates& start, const Coordinates& end) noexcept;
        std::int32_t InsertTextAt(Coordinates& where, const char* value) noexcept;
        void         AddUndo(UndoRecord& value) noexcept;
        [[nodiscard]] Coordinates  ScreenPosToCoordinates(const ImVec2& position) const noexcept;
        [[nodiscard]] Coordinates  FindWordStart(const Coordinates& from) const noexcept;
        [[nodiscard]] Coordinates  FindWordEnd(const Coordinates& from) const noexcept;
        [[nodiscard]] Coordinates  FindNextWord(const Coordinates& from) const noexcept;
        [[nodiscard]] std::int32_t GetCharacterIndex(const Coordinates& coordinates) const noexcept;
        [[nodiscard]] std::int32_t GetCharacterColumn(std::int32_t line_number,
                                                      std::int32_t index) const noexcept;
        [[nodiscard]] std::int32_t GetLineCharacterCount(std::int32_t line) const noexcept;
        [[nodiscard]] std::int32_t GetLineMaxColumn(std::int32_t line) const noexcept;
        [[nodiscard]] bool         IsOnWordBoundary(const Coordinates& at) const noexcept;
        void                       RemoveLine(std::int32_t start, std::int32_t end) noexcept;
        void                       RemoveLine(std::int32_t index) noexcept;
        Line&                      InsertLine(std::int32_t index) noexcept;
        void                       EnterCharacter(ImWchar character, bool shift) noexcept;
        void                       Backspace() noexcept;
        void                       DeleteSelection() noexcept;
        [[nodiscard]] std::string  GetWordUnderCursor() const noexcept;
        [[nodiscard]] std::string  GetWordAt(const Coordinates& coords) const noexcept;
        ImU32                      GetGlyphColor(const Glyph& glyph) const noexcept;

        void HandleKeyboardInputs() noexcept;
        void HandleMouseInputs() noexcept;
        void InternalRender() noexcept;

        void                    RebuildAllTokens() noexcept;
        std::vector<dlx::Token> BuildFullTokenStream() noexcept;
        void                    ColorizeToken(const dlx::Token& token) noexcept;
        void                    ColorizeInternal() noexcept;

        void ResetState() noexcept;

        float       m_LineSpacing;
        EditorState m_State;
        UndoBuffer  m_UndoBuffer;
        std::size_t m_UndoIndex;

        std::uint32_t m_TabSize;
        bool          m_Overwrite : 1;
        bool          m_ReadOnly : 1;
        bool          m_WithinRender : 1;
        bool          m_ScrollToCursor : 1;
        bool          m_ScrollToTop : 1;
        bool          m_TextChanged : 1;
        bool          m_ColorizerEnabled : 1;
        bool          m_CursorPositionChanged : 1;
        float         m_TextStart; // position (in pixels) where a code line starts relative to the left of the CodeEditor.
        std::int32_t  m_LeftMargin;
        std::int32_t  m_ColorRangeMin;
        std::int32_t  m_ColorRangeMax;
        SelectionMode m_SelectionMode;
        bool          m_ShowWhitespaces : 1;

        Palette m_PaletteBase;
        Palette m_Palette;

        Breakpoints  m_Breakpoints;
        ErrorMarkers m_ErrorMarkers;
        ImVec2       m_CharAdvance;
        Coordinates  m_InteractiveStart;
        Coordinates  m_InteractiveEnd;
        std::string  m_LineBuffer;
        uint64_t     m_StartTime;

        float m_LastClick;

        Emulator*   m_Emulator;
        Lines       m_Lines;
        std::string m_FullText;
    };
} // namespace dlxemu
