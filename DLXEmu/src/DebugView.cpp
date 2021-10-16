#include "DLXEmu/DebugView.hpp"

#include "DLXEmu/CodeEditor.hpp"
#include "DLXEmu/Emulator.hpp"
#include "Phi/Config/Warning.hpp"
#include <DLX/Processor.hpp>
#include <Phi/Core/Assert.hpp>
#include <Phi/Core/Log.hpp>
#include <imgui.h>
#include <spdlog/fmt/bundled/core.h>
#include <limits>
#include <random>

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
        }

        ImGui::End();

        // Run test
        if (m_TestGuiMode)
        {
            RunGuiTest();
        }
#endif
    }

    [[nodiscard]] std::string generate_random_code_string(std::mt19937& engine,
                                                          std::size_t   max_length) noexcept
    {
        constexpr static const char possible_characters[]{
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:/;# \t\n"};
        constexpr static std::size_t number_of_possible_characters =
                sizeof(possible_characters) / sizeof(char);

        std::uniform_int_distribution<> dist(0, number_of_possible_characters - 1);

        std::string ret;
        ret.reserve(max_length);

        for (std::size_t i{0}; i < max_length; ++i)
        {
            int random_index = dist(engine);
            ret += possible_characters[random_index];
        }

        return ret;
    }

    [[nodiscard]] std::string generate_random_line(std::mt19937& engine,
                                                   std::size_t   max_length) noexcept
    {
        constexpr static const char possible_characters[]{
                "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz:/;# \t"};
        constexpr static std::size_t number_of_possible_characters =
                sizeof(possible_characters) / sizeof(char);

        std::uniform_int_distribution<> dist(0, number_of_possible_characters - 1);

        std::string ret;
        ret.reserve(max_length);

        for (std::size_t i{0}; i < max_length; ++i)
        {
            int random_index = dist(engine);
            ret += possible_characters[random_index];
        }

        return ret;
    }

    void execute_random_editor_function(std::mt19937& engine, CodeEditor& editor) noexcept
    {
        std::uniform_int_distribution<> function_distrib(0, 31);
        const int                       function_index = function_distrib(engine);

        std::uniform_int_distribution<std::size_t>   message_length_distrib(0, 20);
        std::uniform_int_distribution<std::uint16_t> bool_distrib(0, 1);

        std::uniform_int_distribution<std::int16_t> coord_distrib(
                -1, std::numeric_limits<std::int16_t>::max());
        std::uniform_int_distribution<std::size_t> size_t_distrib(
                std::numeric_limits<std::size_t>::min(), std::numeric_limits<std::size_t>::max());
        std::uniform_int_distribution<std::int32_t> int32_t_distrib(
                std::numeric_limits<std::int32_t>::min(), std::numeric_limits<std::int32_t>::max());
        std::uniform_int_distribution<std::uint32_t> uint32_t_distrib(
                std::numeric_limits<std::uint32_t>::min(),
                std::numeric_limits<std::uint32_t>::max());
        std::uniform_int_distribution<std::int16_t> int16_t_distrib(
                std::numeric_limits<std::int16_t>::min(), std::numeric_limits<std::int16_t>::max());
        std::uniform_int_distribution<std::uint16_t> uint16_t_distrib(
                std::numeric_limits<std::uint16_t>::min(),
                std::numeric_limits<std::uint16_t>::max());

        switch (function_index)
        {
            // AddErrorMarker
            case 0: {
                std::uniform_int_distribution<std::uint32_t> line_number_distrib(
                        0, std::numeric_limits<std::uint32_t>::max());

                std::uint32_t line_number    = line_number_distrib(engine);
                std::size_t   message_length = message_length_distrib(engine);
                std::string   message        = generate_random_line(engine, message_length);

                //PHI_LOG_DEBUG("AddErrorMarker: {:d}, '{:s}'", line_number, message);

                editor.AddErrorMarker(line_number, message);
                break;
            }

            // ClearErrorMarkers
            case 1: {
                //PHI_LOG_DEBUG("ClearErrorMarkers");

                editor.ClearErrorMarkers();

                break;
            }

            // GetText
            case 2: {
                std::string str = editor.GetText();
                PHI_UNUSED_VARIABLE(str);
                //PHI_LOG_DEBUG("GetText: {:s}", str);
                break;
            }

            // SetTextLines
            case 3: {
                std::uniform_int_distribution<std::size_t> number_of_lines_distrib(0, 10);

                std::size_t number_of_lines = number_of_lines_distrib(engine);

                std::vector<std::string> lines;
                lines.reserve(number_of_lines);
                for (std::size_t i{0}; i < number_of_lines; ++i)
                {
                    std::size_t message_length = message_length_distrib(engine);
                    lines.emplace_back(generate_random_line(engine, message_length));
                }

                /*
                PHI_LOG_DEBUG("SetTextLines: {:d}", number_of_lines);
                for (std::size_t i{0}; i < lines.size(); ++i)
                {
                    PHI_LOG_DEBUG("[{:02d}]: {:s}", i, lines.at(i));
                }
                */

                editor.SetTextLines(lines);
                break;
            }

            // GetTextLines
            case 4: {
                std::vector<std::string> lines = editor.GetTextLines();
                PHI_UNUSED_VARIABLE(lines);

                break;
            }

            // GetSelectedText
            case 5: {
                std::string line = editor.GetSelectedText();
                PHI_UNUSED_VARIABLE(line);
                break;
            }

            // GetCurrentLineText
            case 6: {
                std::string line = editor.GetCurrentLineText();
                PHI_UNUSED_VARIABLE(line);
                break;
            }

            // SetReadOnly
            case 7: {
                bool read_only = bool_distrib(engine) == 1;

                editor.SetReadOnly(read_only);
                break;
            }

            // GetCursorPosition
            case 8: {
                CodeEditor::Coordinates coords = editor.GetCursorPosition();
                PHI_UNUSED_VARIABLE(coords);
                break;
            }

            // SetCursorPosition
            case 9: {
                CodeEditor::Coordinates coords;
                coords.m_Column = coord_distrib(engine);
                coords.m_Line   = coord_distrib(engine);

                editor.SetCursorPosition(coords);
                break;
            }

            // SetShowWhitespaces
            case 10: {
                bool show_whitespaces = bool_distrib(engine) == 1;

                editor.SetShowWhitespaces(show_whitespaces);
                break;
            }

            // SetTabSize
            case 11: {
                std::int32_t tab_size = int32_t_distrib(engine);

                editor.SetTabSize(tab_size);
                break;
            }

            // InsertText
            case 12: {
                std::size_t message_length = message_length_distrib(engine);
                std::string text           = generate_random_code_string(engine, message_length);

                //PHI_LOG_DEBUG("InsertText: {:s}", text);

                editor.InsertText(text);
                break;
            }

            // MoveUp
            case 13: {
                std::uint32_t amount = uint32_t_distrib(engine);
                bool          select = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveUp: {:d} {:s}", amount, select ? "True" : "False");

                editor.MoveUp(amount, select);
                break;
            }

            // MoveDown
            case 14: {
                std::uint32_t amount = uint32_t_distrib(engine);
                bool          select = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveDown: {:d} {:s}", amount, select ? "True" : "False");

                editor.MoveDown(amount, select);
                break;
            }

            // MoveLeft
            case 15: {
                std::uint32_t amount    = uint32_t_distrib(engine);
                bool          select    = bool_distrib(engine) == 1;
                bool          word_mode = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveLeft: {:d} {:s} {:s}", amount, select ? "True" : "False",
                //              word_mode ? "True" : "False");

                editor.MoveLeft(amount, select, word_mode);
                break;
            }

            // MoveRight
            case 16: {
                std::uint32_t amount    = uint32_t_distrib(engine);
                bool          select    = bool_distrib(engine) == 1;
                bool          word_mode = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveRight: {:d} {:s} {:s}", amount, select ? "True" : "False",
                //              word_mode ? "True" : "False");

                editor.MoveRight(amount, select, word_mode);
                break;
            }

            // MoveTop
            case 17: {
                bool select = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveTop: {:s}", select ? "True" : "False");

                editor.MoveTop(select);
                break;
            }

            // MoveBottom
            case 18: {
                bool select = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveBottom: {:s}", select ? "True" : "False");

                editor.MoveBottom(select);
                break;
            }

            // MoveHome
            case 19: {
                bool select = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveHome: {:s}", select ? "True" : "False");

                editor.MoveHome(select);
                break;
            }

            // MoveEnd
            case 20: {
                bool select = bool_distrib(engine) == 1;

                //PHI_LOG_DEBUG("MoveEnd: {:s}", select ? "True" : "False");

                editor.MoveEnd(select);
                break;
            }

            // SetSelectionStart
            case 21: {
                std::int32_t column = coord_distrib(engine);
                std::int32_t line   = coord_distrib(engine);

                CodeEditor::Coordinates coord;
                coord.m_Column = column;
                coord.m_Line   = line;

                editor.SetSelectionStart(coord);
                break;
            }

            // SetSelectionEnd
            case 22: {
                std::int32_t column = coord_distrib(engine);
                std::int32_t line   = coord_distrib(engine);

                CodeEditor::Coordinates coord;
                coord.m_Column = column;
                coord.m_Line   = line;

                editor.SetSelectionEnd(coord);
                break;
            }

            // SetSelection
            case 23: {
                std::uniform_int_distribution<std::uint16_t> selection_mode_distrib(0, 2);

                std::int32_t column_start = coord_distrib(engine);
                std::int32_t line_start   = coord_distrib(engine);

                std::int32_t column_end = coord_distrib(engine);
                std::int32_t line_end   = coord_distrib(engine);

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
            case 24: {
                editor.SelectWordUnderCursor();
                break;
            }

            // SelectAll
            case 25: {
                editor.SelectAll();
                break;
            }

            // Copy
            case 26: {
                //PHI_LOG_DEBUG("Copy");

                editor.Copy();
                break;
            }

            // Cut
            case 27: {
                //PHI_LOG_DEBUG("Cut");

                editor.Cut();
                break;
            }

            // Paste
            case 28: {
                //PHI_LOG_DEBUG("Paste");

                editor.Paste();
                break;
            }

            // Delete
            case 29: {
                //PHI_LOG_DEBUG("Delete");

                editor.Delete();
                break;
            }

            // Undo
            case 30: {
                std::uint32_t steps = uint32_t_distrib(engine);

                //PHI_LOG_DEBUG("Undo: {:d}", steps);

                editor.Undo(steps);
                break;
            }

            // Redo
            case 31: {
                std::uint32_t steps = uint32_t_distrib(engine);

                //PHI_LOG_DEBUG("Redo: {:d}", steps);

                editor.Redo(steps);
                break;
            }

            default:
                PHI_ASSERT_NOT_REACHED();
                break;
        }
    }

    void DebugView::RunGuiTest() noexcept
    {
        static std::random_device random_device;
        static std::mt19937       engine(random_device());

        CodeEditor& editor = m_Emulator->m_CodeEditor;

        // Generate random string of length 0-100
        std::uniform_int_distribution<> distrib(0, 100);
        std::string random_text = generate_random_code_string(engine, distrib(engine));

        // Set text
        editor.SetText(random_text);

        // Execute 50 random functions
        for (int i{0}; i < 50; ++i)
        {
            execute_random_editor_function(engine, editor);
        }
    }
} // namespace dlxemu
