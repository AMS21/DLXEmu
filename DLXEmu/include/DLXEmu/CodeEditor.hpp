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

#include <DLX/EnumName.hpp>
#include <DLX/Token.hpp>
#include <imgui.h>
#include <phi/compiler_support/warning.hpp>
#include <phi/container/array.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size);

// TODO: Fully upgrade the lines and columns to 64 bit numbers

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

namespace dlxemu
{
    class Emulator;

    class CodeEditor
    {
    public:
        enum class PaletteIndex : phi::uint8_t
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

        enum class SelectionMode : phi::uint8_t
        {
            Normal,
            Word,
            Line
        };

        class Breakpoint
        {
        public:
            phi::int32_t m_Line{-1};
            phi::boolean m_Enabled{false};
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

            Coordinates(phi::u32 line, phi::u32 column) noexcept;

            [[nodiscard]] phi::boolean operator==(const Coordinates& other) const noexcept;

            [[nodiscard]] phi::boolean operator!=(const Coordinates& other) const noexcept;

            [[nodiscard]] phi::boolean operator<(const Coordinates& other) const noexcept;

            [[nodiscard]] phi::boolean operator>(const Coordinates& other) const noexcept;

            [[nodiscard]] phi::boolean operator<=(const Coordinates& other) const noexcept;

            [[nodiscard]] phi::boolean operator>=(const Coordinates& other) const noexcept;

            phi::u32 m_Line;
            phi::u32 m_Column;
        };

        using ErrorMarkers = std::map<phi::uint32_t, std::string>;
        using Breakpoints  = std::unordered_set<phi::uint32_t>;
        using Palette      = phi::array<ImU32, static_cast<phi::size_t>(PaletteIndex::Max)>;
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

        CodeEditor(const CodeEditor&) = delete;

        ~CodeEditor() noexcept;

        [[nodiscard]] const Palette& GetPalette() const noexcept;
        void                         SetPalette(const Palette& value) noexcept;

        void SetErrorMarkers(const ErrorMarkers& markers) noexcept;
        void AddErrorMarker(const phi::u32 line_number, const std::string& message) noexcept;
        void ClearErrorMarkers() noexcept;
        [[nodiscard]] ErrorMarkers&       GetErrorMarkers() noexcept;
        [[nodiscard]] const ErrorMarkers& GetErrorMarkers() const noexcept;

        void                             SetBreakpoints(const Breakpoints& markers) noexcept;
        phi::boolean                     AddBreakpoint(const phi::u32 line_number) noexcept;
        phi::boolean                     RemoveBreakpoint(const phi::u32 line_number) noexcept;
        phi::boolean                     ToggleBreakpoint(const phi::u32 line_number) noexcept;
        void                             ClearBreakPoints() noexcept;
        [[nodiscard]] Breakpoints&       GetBreakpoints() noexcept;
        [[nodiscard]] const Breakpoints& GetBreakpoints() const noexcept;

        void Render(const ImVec2& size = ImVec2(), phi::boolean border = false) noexcept;

        void                      SetText(const std::string& text) noexcept;
        [[nodiscard]] std::string GetText() const noexcept;
        void                      ClearText() noexcept;

        void SetTextLines(const std::vector<std::string>& lines) noexcept;
        [[nodiscard]] std::vector<std::string> GetTextLines() const noexcept;

        [[nodiscard]] std::string GetSelectedText() const noexcept;
        [[nodiscard]] std::string GetCurrentLineText() const noexcept;

        [[nodiscard]] phi::usize GetTotalLines() const noexcept;

        void                       SetOverwrite(phi::boolean overwrite) noexcept;
        void                       ToggleOverwrite() noexcept;
        [[nodiscard]] phi::boolean IsOverwrite() const noexcept;

        void                       SetReadOnly(phi::boolean value) noexcept;
        void                       ToggleReadOnly() noexcept;
        [[nodiscard]] phi::boolean IsReadOnly() const noexcept;

        [[nodiscard]] phi::boolean IsTextChanged() const noexcept;
        [[nodiscard]] phi::boolean IsCursorPositionChanged() const noexcept;

        void                       SetColorizerEnable(phi::boolean value) noexcept;
        void                       ToggleColorizerEnabled() noexcept;
        [[nodiscard]] phi::boolean IsColorizerEnabled() const noexcept;

        [[nodiscard]] Coordinates GetCursorPosition() const noexcept;
        void                      SetCursorPosition(const Coordinates& position) noexcept;

        void                       SetShowWhitespaces(phi::boolean value) noexcept;
        void                       ToggleShowWhitespaces() noexcept;
        [[nodiscard]] phi::boolean IsShowingWhitespaces() const noexcept;

        void                       SetTabSize(phi::u8_fast new_tab_size) noexcept;
        [[nodiscard]] phi::u8_fast GetTabSize() const noexcept;

        void EnterCharacter(ImWchar character, phi::boolean shift = false) noexcept;
        void Backspace() noexcept;

        void InsertText(const std::string& value) noexcept;
        void InsertText(const char* value) noexcept;

        void MoveUp(phi::u32 amount = 1u, phi::boolean select = false) noexcept;
        void MoveDown(phi::u32 amount = 1u, phi::boolean select = false) noexcept;
        void MoveLeft(phi::u32 amount = 1u, phi::boolean select = false,
                      phi::boolean word_mode = false) noexcept;
        void MoveRight(phi::u32 amount = 1u, phi::boolean select = false,
                       phi::boolean word_mode = false) noexcept;
        void MoveTop(phi::boolean select = false) noexcept;
        void MoveBottom(phi::boolean select = false) noexcept;
        void MoveHome(phi::boolean select = false) noexcept;
        void MoveEnd(phi::boolean select = false) noexcept;

        void                       SetSelectionStart(const Coordinates& position) noexcept;
        void                       SetSelectionEnd(const Coordinates& position) noexcept;
        void                       SetSelection(const Coordinates& start, const Coordinates& end,
                                                SelectionMode mode = SelectionMode::Normal) noexcept;
        void                       SelectWordUnderCursor() noexcept;
        void                       SelectAll() noexcept;
        void                       ClearSelection() noexcept;
        [[nodiscard]] phi::boolean HasSelection() const noexcept;
        [[nodiscard]] Coordinates  GetSelectionStart() const noexcept;
        [[nodiscard]] Coordinates  GetSelectionEnd() const noexcept;

        void Copy() const noexcept;
        void Cut() noexcept;
        void Paste() noexcept;
        void Delete() noexcept;

        [[nodiscard]] phi::boolean CanUndo() const noexcept;
        void                       Undo(phi::u32 steps = 1u) noexcept;

        [[nodiscard]] phi::boolean CanRedo() const noexcept;
        void                       Redo(phi::u32 steps = 1u) noexcept;

        [[nodiscard]] std::string GetEditorDump() const noexcept;

        void UpdatePalette() noexcept;

        void VerifyInternalState() const noexcept;

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

            phi::boolean operator==(const EditorState& other) const noexcept;

            phi::boolean operator!=(const EditorState& other) const noexcept;
        };

        class UndoRecord
        {
        public:
            UndoRecord() = default;

            void Undo(CodeEditor* editor) const noexcept;
            void Redo(CodeEditor* editor) const noexcept;

            void StoreBeforeState(CodeEditor* editor) noexcept;
            void StoreAfterState(CodeEditor* editor) noexcept;

            void ApplyBeforeState(CodeEditor* editor) const noexcept;
            void ApplyAfterState(CodeEditor* editor) const noexcept;

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

        void                Colorize(phi::u32 from_line = 0u, phi::i64 count = -1) noexcept;
        [[nodiscard]] float TextDistanceToLineStart(const Coordinates& from) const noexcept;

        void EnsureCursorVisible() noexcept;
        void ScrollToCursor() noexcept;

        [[nodiscard]] phi::u32    GetPageSize() const noexcept;
        [[nodiscard]] std::string GetText(const Coordinates& start,
                                          const Coordinates& end) const noexcept;
        [[nodiscard]] Coordinates GetActualCursorCoordinates() const noexcept;
        [[nodiscard]] Coordinates SanitizeCoordinates(const Coordinates& value) const noexcept;
        void                      Advance(Coordinates& coordinates) const noexcept;

        void DeleteRange(const Coordinates& start, const Coordinates& end) noexcept;

        phi::u32 InsertTextAt(Coordinates& where, const char* value) noexcept;

        void AddUndo(UndoRecord& value) noexcept;

        [[nodiscard]] Coordinates ScreenPosToCoordinates(const ImVec2& position) const noexcept;
        [[nodiscard]] Coordinates FindWordStart(const Coordinates& from) const noexcept;
        [[nodiscard]] Coordinates FindWordEnd(const Coordinates& from) const noexcept;
        [[nodiscard]] Coordinates FindNextWord(const Coordinates& from) const noexcept;

        [[nodiscard]] std::string GetWordUnderCursor() const noexcept;
        [[nodiscard]] std::string GetWordAt(const Coordinates& coords) const noexcept;

        [[nodiscard]] phi::u32 GetCharacterIndex(const Coordinates& coordinates) const noexcept;
        [[nodiscard]] phi::u32 GetCharacterColumn(phi::u32 line_number,
                                                  phi::u32 index) const noexcept;
        [[nodiscard]] phi::u32 GetLineCharacterCount(phi::u32 line) const noexcept;
        [[nodiscard]] phi::u32 GetLineMaxColumn(phi::u32 line) const noexcept;
        [[nodiscard]] phi::u32 GetMaxLineNumber() const noexcept;

        [[nodiscard]] phi::boolean IsOnWordBoundary(const Coordinates& at) const noexcept;

        void  RemoveLine(phi::u32 start, phi::u32 end) noexcept;
        void  RemoveLine(phi::u32 index) noexcept;
        Line& InsertLine(phi::u32 index) noexcept;

        void EnterCharacterImpl(ImWchar character, phi::boolean shift) noexcept;

        void BackspaceImpl() noexcept;
        void DeleteSelection() noexcept;

        [[nodiscard]] ImU32 GetGlyphColor(const Glyph& glyph) const noexcept;

        void HandleKeyboardInputs() noexcept;
        void HandleMouseInputs() noexcept;

        void InternalRender() noexcept;

        void ColorizeToken(const dlx::Token& token) noexcept;
        void ColorizeInternal() noexcept;

        [[nodiscard]] phi::u8_fast GetTabSizeAt(phi::u32 column) const noexcept;
        [[nodiscard]] ImU32        GetPaletteForIndex(PaletteIndex index) const noexcept;

        void ResetState() noexcept;
        void FixSelectionAfterMove(phi::boolean select, Coordinates old_pos) noexcept;

        void ComputeCharAdvance() noexcept;

        float       m_LineSpacing;
        EditorState m_State;
        UndoBuffer  m_UndoBuffer;
        phi::usize  m_UndoIndex;

        phi::u8_fast m_TabSize;
        phi::boolean m_Overwrite;
        phi::boolean m_ReadOnly;
        phi::boolean m_ScrollToCursor;
        phi::boolean m_ScrollToTop;
        phi::boolean m_TextChanged;
        phi::boolean m_ColorizerEnabled;
        phi::boolean m_CursorPositionChanged;
        float m_TextStart; // position (in pixels) where a code line starts relative to the left of the CodeEditor.
        phi::u32      m_ColorRangeMin;
        phi::u32      m_ColorRangeMax;
        SelectionMode m_SelectionMode;
        phi::boolean  m_ShowWhitespaces;

        Palette m_PaletteBase;
        Palette m_Palette;

        Breakpoints  m_Breakpoints;
        ErrorMarkers m_ErrorMarkers;
        ImVec2       m_CharAdvance;
        Coordinates  m_InteractiveStart;
        Coordinates  m_InteractiveEnd;
        std::string  m_LineBuffer;
        uint64_t     m_StartTime;

        // TODO: Where saving a float but ImGui returns a double
        float m_LastClick;

        Emulator*   m_Emulator;
        Lines       m_Lines;
        std::string m_FullText;

        // Constants
        static const constexpr phi::u8_fast MinTabSize{static_cast<phi::uint_fast8_t>(1u)};
        static const constexpr phi::u8_fast MaxTabSize{static_cast<phi::uint_fast8_t>(32u)};

        static const constexpr float LeftMargin{10.0f};

        // Declare fuzzer function as a friend so it can access all members and functions
        friend int ::LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size);
    };
} // namespace dlxemu

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wcovered-switch-default")
PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wreturn-type")

namespace dlx
{
    template <>
    [[nodiscard]] constexpr std::string_view enum_name<dlxemu::CodeEditor::SelectionMode>(
            dlxemu::CodeEditor::SelectionMode value) noexcept
    {
        switch (value)
        {
            case dlxemu::CodeEditor::SelectionMode::Line:
                return "Line";
            case dlxemu::CodeEditor::SelectionMode::Normal:
                return "Normal";
            case dlxemu::CodeEditor::SelectionMode::Word:
                return "Word";

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }
} // namespace dlx

PHI_GCC_SUPPRESS_WARNING_POP()
PHI_CLANG_SUPPRESS_WARNING_POP()
