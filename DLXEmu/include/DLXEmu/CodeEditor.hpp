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
            CharLiteral,
            Punctuation,
            Preprocessor,
            Identifier,
            KnownIdentifier,
            PreprocIdentifier,
            Comment,
            MultiLineComment,
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
            Coordinates();

            Coordinates(std::int32_t line, std::int32_t column);

            [[nodiscard]] static Coordinates Invalid();

            bool operator==(const Coordinates& o) const;

            bool operator!=(const Coordinates& o) const;

            bool operator<(const Coordinates& o) const;

            bool operator>(const Coordinates& o) const;

            bool operator<=(const Coordinates& o) const;

            bool operator>=(const Coordinates& o) const;

            std::int32_t m_Line;
            std::int32_t m_Column;
        };

        class Identifier
        {
        public:
            Coordinates m_Location;
            std::string m_Declaration;
        };

        using ErrorMarkers = std::map<std::int32_t, std::string>;
        using Breakpoints  = std::unordered_set<std::int32_t>;
        using Palette      = std::array<ImU32, static_cast<std::size_t>(PaletteIndex::Max)>;
        using Char         = uint8_t;

        class Glyph
        {
        public:
            Glyph(Char character, PaletteIndex color_index);

            Char         m_Char;
            PaletteIndex m_ColorIndex = PaletteIndex::Default;
        };

        using Line  = std::vector<Glyph>;
        using Lines = std::vector<Line>;

        CodeEditor(Emulator* emulator);

        [[nodiscard]] const Palette& GetPalette() const;
        void                         SetPalette(const Palette& value);

        void SetErrorMarkers(const ErrorMarkers& markers);

        void SetBreakpoints(const Breakpoints& markers);

        void                      Render(const ImVec2& size = ImVec2(), bool border = false);
        void                      SetText(const std::string& text);
        [[nodiscard]] std::string GetText() const;

        void                                   SetTextLines(const std::vector<std::string>& lines);
        [[nodiscard]] std::vector<std::string> GetTextLines() const;

        [[nodiscard]] std::string GetSelectedText() const;
        [[nodiscard]] std::string GetCurrentLineText() const;

        std::uint32_t      GetTotalLines() const;
        [[nodiscard]] bool IsOverwrite() const;

        void               SetReadOnly(bool value);
        [[nodiscard]] bool IsReadOnly() const;
        [[nodiscard]] bool IsTextChanged() const;
        [[nodiscard]] bool IsCursorPositionChanged() const;

        [[nodiscard]] bool IsColorizerEnabled() const;
        void               SetColorizerEnable(bool value);

        [[nodiscard]] Coordinates GetCursorPosition() const;
        void                      SetCursorPosition(const Coordinates& position);

        void               SetHandleMouseInputs(bool value);
        [[nodiscard]] bool IsHandleMouseInputsEnabled() const;

        void               SetHandleKeyboardInputs(bool value);
        [[nodiscard]] bool IsHandleKeyboardInputsEnabled() const;

        void               SetImGuiChildIgnored(bool value);
        [[nodiscard]] bool IsImGuiChildIgnored() const;

        void               SetShowWhitespaces(bool value);
        [[nodiscard]] bool IsShowingWhitespaces() const;

        void                        SetTabSize(std::int32_t value);
        [[nodiscard]] std::uint32_t GetTabSize() const;

        void InsertText(const std::string& value);
        void InsertText(const char* value);

        void MoveUp(std::int32_t amount = 1, bool select = false);
        void MoveDown(std::int32_t amount = 1, bool select = false);
        void MoveLeft(std::int32_t amount = 1, bool select = false, bool word_mode = false);
        void MoveRight(std::int32_t amount = 1, bool select = false, bool word_mode = false);
        void MoveTop(bool select = false);
        void MoveBottom(bool select = false);
        void MoveHome(bool select = false);
        void MoveEnd(bool select = false);

        void               SetSelectionStart(const Coordinates& position);
        void               SetSelectionEnd(const Coordinates& position);
        void               SetSelection(const Coordinates& start, const Coordinates& end,
                                        SelectionMode mode = SelectionMode::Normal);
        void               SelectWordUnderCursor();
        void               SelectAll();
        [[nodiscard]] bool HasSelection() const;

        void Copy();
        void Cut();
        void Paste();
        void Delete();

        [[nodiscard]] bool CanUndo() const;
        [[nodiscard]] bool CanRedo() const;
        void               Undo(std::int32_t steps = 1);
        void               Redo(std::int32_t steps = 1);

        static const Palette& GetDarkPalette();
        static const Palette& GetLightPalette();
        static const Palette& GetRetroBluePalette();

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

                       CodeEditor::EditorState& before, CodeEditor::EditorState& after);

            void Undo(CodeEditor* editor);
            void Redo(CodeEditor* editor);

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

        void                       ProcessInputs();
        void                       Colorize(std::int32_t from_line = 0, std::int32_t count = -1);
        [[nodiscard]] float        TextDistanceToLineStart(const Coordinates& from) const;
        void                       EnsureCursorVisible();
        [[nodiscard]] std::int32_t GetPageSize() const;
        [[nodiscard]] std::string  GetText(const Coordinates& start, const Coordinates& end) const;
        [[nodiscard]] Coordinates  GetActualCursorCoordinates() const;
        [[nodiscard]] Coordinates  SanitizeCoordinates(const Coordinates& value) const;
        void                       Advance(Coordinates& coordinates) const;
        void                       DeleteRange(const Coordinates& start, const Coordinates& end);
        std::int32_t               InsertTextAt(Coordinates& where, const char* value);
        void                       AddUndo(UndoRecord& value);
        [[nodiscard]] Coordinates  ScreenPosToCoordinates(const ImVec2& position) const;
        [[nodiscard]] Coordinates  FindWordStart(const Coordinates& from) const;
        [[nodiscard]] Coordinates  FindWordEnd(const Coordinates& from) const;
        [[nodiscard]] Coordinates  FindNextWord(const Coordinates& from) const;
        [[nodiscard]] std::int32_t GetCharacterIndex(const Coordinates& coordinates) const;
        [[nodiscard]] std::int32_t GetCharacterColumn(std::int32_t line_number,
                                                      std::int32_t index) const;
        [[nodiscard]] std::int32_t GetLineCharacterCount(std::int32_t line) const;
        [[nodiscard]] std::int32_t GetLineMaxColumn(std::int32_t line) const;
        [[nodiscard]] bool         IsOnWordBoundary(const Coordinates& at) const;
        void                       RemoveLine(std::int32_t start, std::int32_t end);
        void                       RemoveLine(std::int32_t index);
        Line&                      InsertLine(std::int32_t index);
        void                       EnterCharacter(ImWchar character, bool shift);
        void                       Backspace();
        void                       DeleteSelection();
        [[nodiscard]] std::string  GetWordUnderCursor() const;
        [[nodiscard]] std::string  GetWordAt(const Coordinates& coords) const;
        ImU32                      GetGlyphColor(const Glyph& glyph) const;

        void HandleKeyboardInputs();
        void HandleMouseInputs();
        void InternalRender();

        void                    RebuildAllTokens();
        std::vector<dlx::Token> BuildFullTokenStream();
        void                    ColorizeToken(const dlx::Token& token);
        void                    ColorizeInternal();

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
        bool          m_HandleKeyboardInputs : 1;
        bool          m_HandleMouseInputs : 1;
        bool          m_IgnoreImGuiChild : 1;
        bool          m_ShowWhitespaces : 1;
        bool          m_CheckComments : 1;

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
