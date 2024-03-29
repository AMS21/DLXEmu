#include "DLXEmu/DebugView.hpp"

#include "DLXEmu/CodeEditor.hpp"
#include "DLXEmu/Emulator.hpp"
#include <DLX/Logger.hpp>
#include <DLX/Processor.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5262)

#include <imgui.h>
#include <limits>
#include <random>
#include <vector>

PHI_MSVC_SUPPRESS_WARNING_POP()

PHI_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")

namespace dlxemu
{
    DebugView::DebugView(Emulator* emulator) noexcept
        : m_Emulator{emulator}
    {}

    void DebugView::Render() noexcept
    {
#if defined(PHI_DEBUG)
        if (ImGui::Begin("Debug View", &m_Emulator->m_ShowDebugView))
        {
            const dlx::Processor& processor = m_Emulator->GetProcessor();

            ImGui::Checkbox("GUI test mode", &m_TestGuiMode);

            // Dumps
            if (ImGui::CollapsingHeader("Processor Dump"))
            {
                const std::string dump = processor.GetProcessorDump();
                ImGui::TextUnformatted(dump.c_str());
            }

            if (ImGui::CollapsingHeader("Register Dump"))
            {
                const std::string dump = processor.GetRegisterDump();
                ImGui::TextUnformatted(dump.c_str());
            }

            if (ImGui::CollapsingHeader("Memory Dump"))
            {
                const std::string dump = processor.GetMemoryDump();
                ImGui::TextUnformatted(dump.c_str());
            }

            if (ImGui::CollapsingHeader("Program Dump"))
            {
                const std::string dump = m_Emulator->m_DLXProgram.GetDump();
                ImGui::TextUnformatted(dump.c_str());
            }

            if (ImGui::CollapsingHeader("Editor Dump"))
            {
                const std::string dump = m_Emulator->m_CodeEditor.GetEditorDump();
                ImGui::TextUnformatted(dump.c_str());
            }
        }

        ImGui::End();

        // Run test
        if (m_TestGuiMode)
        {
            RunGuiTest();
        }
#endif
    }

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

    [[nodiscard]] std::string generate_random_code_string(std::mt19937& engine,
                                                          phi::size_t   max_length) noexcept
    {
        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5264) // Unused variable?

        static constexpr const char possible_characters[]{
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:/;# \t\n"};
        static constexpr const phi::size_t number_of_possible_characters =
                sizeof(possible_characters) / sizeof(char);

        PHI_MSVC_SUPPRESS_WARNING_POP()

        std::uniform_int_distribution<phi::size_t> dist(0u, number_of_possible_characters - 1);

        std::string ret;
        ret.reserve(max_length);

        for (phi::size_t i{0}; i < max_length; ++i)
        {
            const phi::size_t random_index = dist(engine);
            ret += possible_characters[random_index];
        }

        return ret;
    }

    [[nodiscard]] std::string generate_random_line(std::mt19937& engine,
                                                   phi::size_t   max_length) noexcept
    {
        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5264) // Unused variable?

        static constexpr const char possible_characters[]{
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:/;# \t"};
        static constexpr const phi::size_t number_of_possible_characters =
                sizeof(possible_characters) / sizeof(char);

        PHI_MSVC_SUPPRESS_WARNING_POP()

        std::uniform_int_distribution<phi::size_t> dist(0u, number_of_possible_characters - 1u);

        std::string ret;
        ret.reserve(max_length);

        for (phi::size_t i{0}; i < max_length; ++i)
        {
            const phi::size_t random_index = dist(engine);
            ret += possible_characters[random_index];
        }

        return ret;
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

    void execute_random_editor_function(std::mt19937& engine, CodeEditor& editor) noexcept
    {
        std::uniform_int_distribution<> function_distrib(0, 36);
        const int                       function_index = function_distrib(engine);

        std::uniform_int_distribution<phi::size_t>   message_length_distrib(0, 20);
        std::uniform_int_distribution<phi::uint16_t> bool_distrib(0, 1);

        std::uniform_int_distribution<phi::uint32_t> coord_distrib(
                0, std::numeric_limits<phi::uint32_t>::max());
        std::uniform_int_distribution<phi::size_t> size_t_distrib(
                std::numeric_limits<phi::size_t>::min(), std::numeric_limits<phi::size_t>::max());
        std::uniform_int_distribution<phi::int32_t> int32_t_distrib(
                std::numeric_limits<phi::int32_t>::min(), std::numeric_limits<phi::int32_t>::max());
        std::uniform_int_distribution<phi::uint32_t> uint32_t_distrib(
                std::numeric_limits<phi::uint32_t>::min(),
                std::numeric_limits<phi::uint32_t>::max());
        std::uniform_int_distribution<phi::int16_t> int16_t_distrib(
                std::numeric_limits<phi::int16_t>::min(), std::numeric_limits<phi::int16_t>::max());
        std::uniform_int_distribution<phi::uint16_t> uint16_t_distrib(
                std::numeric_limits<phi::uint16_t>::min(),
                std::numeric_limits<phi::uint16_t>::max());
        std::uniform_int_distribution<ImWchar> imwchar_distrib(std::numeric_limits<ImWchar>::min(),
                                                               std::numeric_limits<ImWchar>::max());

        switch (function_index)
        {
            // AddErrorMarker
            case 0: {
                std::uniform_int_distribution<phi::uint32_t> line_number_distrib(
                        0, std::numeric_limits<phi::uint32_t>::max());

                phi::uint32_t line_number    = line_number_distrib(engine);
                phi::size_t   message_length = message_length_distrib(engine);
                std::string   message        = generate_random_line(engine, message_length);

                DLX_DEBUG("AddErrorMarker: {:d}, '{:s}'", line_number, message);

                editor.AddErrorMarker(line_number, message);
                break;
            }

            // ClearErrorMarkers
            case 1: {
                DLX_DEBUG("ClearErrorMarkers");

                editor.ClearErrorMarkers();

                break;
            }

            // SetText
            case 2: {
                std::string str = generate_random_code_string(engine, 100);

                DLX_DEBUG("SetText: {:s}", str);

                editor.SetText(str);
                break;
            }

            // GetText
            case 3: {
                DLX_DEBUG("GetText");

                volatile std::string str = editor.GetText();
                PHI_UNUSED_VARIABLE(str);
                break;
            }

            // SetTextLines
            case 4: {
                std::uniform_int_distribution<phi::size_t> number_of_lines_distrib(0, 10);

                phi::size_t number_of_lines = number_of_lines_distrib(engine);

                std::vector<std::string> lines;
                lines.reserve(number_of_lines);
                for (phi::size_t i{0}; i < number_of_lines; ++i)
                {
                    phi::size_t message_length = message_length_distrib(engine);
                    lines.emplace_back(generate_random_line(engine, message_length));
                }

                /*
                DLX_DEBUG("SetTextLines: {:d}", number_of_lines);
                for (phi::size_t i{0}; i < lines.size(); ++i)
                {
                    DLX_DEBUG("[{:02d}]: {:s}", i, lines.at(i));
                }
                */

                editor.SetTextLines(lines);
                break;
            }

            // GetTextLines
            case 5: {
                std::vector<std::string> lines = editor.GetTextLines();
                PHI_UNUSED_VARIABLE(lines);

                break;
            }

            // GetSelectedText
            case 6: {
                std::string line = editor.GetSelectedText();
                PHI_UNUSED_VARIABLE(line);
                break;
            }

            // GetCurrentLineText
            case 7: {
                std::string line = editor.GetCurrentLineText();
                PHI_UNUSED_VARIABLE(line);
                break;
            }

            // SetReadOnly
            case 8: {
                bool read_only = bool_distrib(engine) == 1;

                editor.SetReadOnly(read_only);
                break;
            }

            // GetCursorPosition
            case 9: {
                CodeEditor::Coordinates coords = editor.GetCursorPosition();
                PHI_UNUSED_VARIABLE(coords);
                break;
            }

            // SetCursorPosition
            case 10: {
                CodeEditor::Coordinates coords;
                coords.m_Column = coord_distrib(engine);
                coords.m_Line   = coord_distrib(engine);

                editor.SetCursorPosition(coords);
                break;
            }

            // SetShowWhitespaces
            case 11: {
                bool show_whitespaces = bool_distrib(engine) == 1;

                editor.SetShowWhitespaces(show_whitespaces);
                break;
            }

            // SetTabSize
            case 12: {
                phi::int16_t tab_size = int16_t_distrib(engine);

                editor.SetTabSize(static_cast<phi::uint8_t>(tab_size));
                break;
            }

            // InsertText
            case 13: {
                phi::size_t message_length = message_length_distrib(engine);
                std::string text           = generate_random_code_string(engine, message_length);

                DLX_DEBUG("InsertText: {:s}", text);

                editor.InsertText(text);
                break;
            }

            // MoveUp
            case 14: {
                phi::uint32_t amount = uint32_t_distrib(engine);
                bool          select = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveUp: {:d} {:s}", amount, select ? "True" : "False");

                editor.MoveUp(amount, select);
                break;
            }

            // MoveDown
            case 15: {
                phi::uint32_t amount = uint32_t_distrib(engine);
                bool          select = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveDown: {:d} {:s}", amount, select ? "True" : "False");

                editor.MoveDown(amount, select);
                break;
            }

            // MoveLeft
            case 16: {
                phi::uint32_t amount    = uint32_t_distrib(engine);
                bool          select    = bool_distrib(engine) == 1;
                bool          word_mode = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveLeft: {:d} {:s} {:s}", amount, select ? "True" : "False",
                          word_mode ? "True" : "False");

                editor.MoveLeft(amount, select, word_mode);
                break;
            }

            // MoveRight
            case 17: {
                phi::uint32_t amount    = uint32_t_distrib(engine);
                bool          select    = bool_distrib(engine) == 1;
                bool          word_mode = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveRight: {:d} {:s} {:s}", amount, select ? "True" : "False",
                          word_mode ? "True" : "False");

                editor.MoveRight(amount, select, word_mode);
                break;
            }

            // MoveTop
            case 18: {
                bool select = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveTop: {:s}", select ? "True" : "False");

                editor.MoveTop(select);
                break;
            }

            // MoveBottom
            case 19: {
                bool select = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveBottom: {:s}", select ? "True" : "False");

                editor.MoveBottom(select);
                break;
            }

            // MoveHome
            case 20: {
                bool select = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveHome: {:s}", select ? "True" : "False");

                editor.MoveHome(select);
                break;
            }

            // MoveEnd
            case 21: {
                bool select = bool_distrib(engine) == 1;

                DLX_DEBUG("MoveEnd: {:s}", select ? "True" : "False");

                editor.MoveEnd(select);
                break;
            }

            // SetSelectionStart
            case 22: {
                phi::uint32_t column = coord_distrib(engine);
                phi::uint32_t line   = coord_distrib(engine);

                CodeEditor::Coordinates coord;
                coord.m_Column = column;
                coord.m_Line   = line;

                editor.SetSelectionStart(coord);
                break;
            }

            // SetSelectionEnd
            case 23: {
                phi::uint32_t column = coord_distrib(engine);
                phi::uint32_t line   = coord_distrib(engine);

                CodeEditor::Coordinates coord;
                coord.m_Column = column;
                coord.m_Line   = line;

                editor.SetSelectionEnd(coord);
                break;
            }

            // SetSelection
            case 24: {
                std::uniform_int_distribution<phi::uint16_t> selection_mode_distrib(0, 2);

                phi::uint32_t column_start = coord_distrib(engine);
                phi::uint32_t line_start   = coord_distrib(engine);

                phi::uint32_t column_end = coord_distrib(engine);
                phi::uint32_t line_end   = coord_distrib(engine);

                CodeEditor::Coordinates coord_start;
                coord_start.m_Column = column_start;
                coord_start.m_Line   = line_start;

                CodeEditor::Coordinates coord_end;
                coord_end.m_Column = column_end;
                coord_end.m_Line   = line_end;

                CodeEditor::SelectionMode selection_mode =
                        static_cast<CodeEditor::SelectionMode>(selection_mode_distrib(engine));

                editor.SetSelection(coord_start, coord_end, selection_mode);
                break;
            }

            // SelectWordUnderCursor
            case 25: {
                DLX_DEBUG("SelectWordUnderCursor");

                editor.SelectWordUnderCursor();
                break;
            }

            // SelectAll
            case 26: {
                DLX_DEBUG("SelectAll");

                editor.SelectAll();
                break;
            }

            // Delete
            case 27: {
                DLX_DEBUG("Delete");

                editor.Delete();
                break;
            }

            // Undo
            case 28: {
                DLX_DEBUG("Undo");

                editor.Undo();
                break;
            }

            // Redo
            case 29: {
                DLX_DEBUG("Redo");

                editor.Redo();
                break;
            }

            // SetErrorMarkers
            case 30: {
                const phi::uint32_t      amount = uint32_t_distrib(engine);
                CodeEditor::ErrorMarkers markers;
                for (phi::size_t i{0u}; i < amount; ++i)
                {
                    phi::uint32_t line_number = uint32_t_distrib(engine);
                    std::string   message     = generate_random_line(engine, 100u);

                    markers[line_number] = message;
                }

                DLX_DEBUG("SetErrorMarkers()");

                editor.SetErrorMarkers(markers);
                break;
            }

            // SetBreakpoints
            case 31: {
                const phi::uint32_t     amount = uint32_t_distrib(engine);
                CodeEditor::Breakpoints breakpoints;

                for (phi::size_t i{0u}; i < amount; ++i)
                {
                    phi::uint32_t line_number = uint32_t_distrib(engine);

                    breakpoints.insert(line_number);
                }

                DLX_DEBUG("SetBreakpoints()");

                editor.SetBreakpoints(breakpoints);
                break;
            }

            // Render
            case 32: {
                break;
            }

            // EnterCharacter
            case 33: {
                const ImWchar character = imwchar_distrib(engine);
                const bool    shift     = bool_distrib(engine) == 1;

                DLX_DEBUG("EnterCharacter({} (\0x{:02X}) {:s})", static_cast<char>(character),
                          static_cast<phi::uint32_t>(character), shift ? "True" : "False");
                editor.EnterCharacter(character, shift);
                break;
            }

            // ClearText
            case 34: {
                DLX_DEBUG("ClearText");

                editor.ClearText();
                break;
            }

            // ClearSelection
            case 35: {
                DLX_DEBUG("ClearSelection");

                editor.ClearSelection();
                break;
            }

            // Backspace
            case 36: {
                DLX_DEBUG("Backspace");

                editor.Backspace();
                break;
            }

            default:
                PHI_ASSERT_NOT_REACHED();
                break;
        }
    }

    void DebugView::RunGuiTest() noexcept
    {
        PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wexit-time-destructors")

        static std::random_device random_device;
        static std::mt19937       engine(random_device());

        PHI_CLANG_SUPPRESS_WARNING_POP()

        CodeEditor& editor = m_Emulator->m_CodeEditor;

        // Execute 50 random functions
        for (int i{0}; i < 50; ++i)
        {
            execute_random_editor_function(engine, editor);
        }
    }
} // namespace dlxemu
