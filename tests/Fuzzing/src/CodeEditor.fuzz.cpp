#include <DLX/Logger.hpp>
#include <DLXEmu/CodeEditor.hpp>
#include <DLXEmu/Emulator.hpp>
#include <fmt/core.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <phi/compiler_support/assume.hpp>
#include <phi/compiler_support/unused.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/scope_guard.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <phi/math/abs.hpp>
#include <phi/math/is_nan.hpp>
#include <phi/preprocessor/function_like_macro.hpp>
#include <phi/type_traits/make_unsigned.hpp>
#include <phi/type_traits/to_unsafe.hpp>
#include <phi/type_traits/underlying_type.hpp>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#if defined(FUZZ_VERBOSE_LOG)
#    define FUZZ_LOG(...)                                                                          \
        fmt::print(stderr, __VA_ARGS__);                                                           \
        std::putc('\n', stderr);                                                                   \
        std::fflush(stderr)
#else
#    define FUZZ_LOG(...) PHI_EMPTY_MACRO()
#endif

#define GET_T(type, name)                                                                          \
    auto PHI_GLUE(name, _opt) = consume_t<type>(data, size, index);                                \
    if (!PHI_GLUE(name, _opt))                                                                     \
    {                                                                                              \
        return {};                                                                                 \
    }                                                                                              \
    const type name = PHI_GLUE(name, _opt).value()

#define GET_T_COND(type, name, cond)                                                               \
    GET_T(type, name);                                                                             \
    if (!(cond))                                                                                   \
    {                                                                                              \
        return {};                                                                                 \
    }                                                                                              \
    (void)(0)

PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")

// TODO: Use string_view as much as possible so we avoid needless copies

// Limits
static constexpr const phi::size_t MaxVectorSize{8u};
static constexpr const phi::size_t MaxStringLength{16u};
static constexpr const float       MaxSaneFloatValue{1024.0f};

struct Cache
{
    using svec = std::vector<std::string>;

    svec vector_string[MaxVectorSize];

    std::string string;

    static Cache Initialize() noexcept
    {
        Cache c;

        // Resize vector args
        for (phi::usize i{0u}; i < MaxVectorSize; ++i)
        {
            svec& vector = c.vector_string[i.unsafe()];
            vector.resize(i.unsafe());

            // Reserve max size
            for (std::string& str : vector)
            {
                str.reserve(MaxStringLength);
            }
        }

        c.string.reserve(MaxStringLength);

        return c;
    }
};

static Cache cache = Cache::Initialize();

[[nodiscard]] constexpr bool has_x_more(const std::size_t index, const std::size_t x,
                                        const std::size_t size) noexcept
{
    return index + x < size;
}

template <typename T>
[[nodiscard]] constexpr std::size_t aligned_size() noexcept
{
    return sizeof(T) + (sizeof(void*) - sizeof(T));
}

template <typename T>
[[nodiscard]] phi::optional<T> consume_t(const std::uint8_t* data, const std::size_t size,
                                         std::size_t& index) noexcept
{
    if (!has_x_more(index, sizeof(T), size))
    {
        return {};
    }

    PHI_ASSUME(index % sizeof(void*) == 0);

    const phi::size_t old_index = index;
    index += aligned_size<T>();

    if constexpr (phi::is_bool_v<T>)
    {
        phi::int8_t value = *reinterpret_cast<const phi::int8_t*>(data + old_index);
        return static_cast<bool>(value);
    }
    else
    {
        return *reinterpret_cast<const T*>(data + old_index);
    }
}

template <>
[[nodiscard]] phi::optional<dlxemu::CodeEditor::Coordinates> consume_t<
        dlxemu::CodeEditor::Coordinates>(const std::uint8_t* data, const std::size_t size,
                                         std::size_t& index) noexcept
{
    GET_T(phi::uint32_t, column);
    GET_T(phi::uint32_t, line);

    return dlxemu::CodeEditor::Coordinates{column, line};
}

[[nodiscard]] bool consume_string(const std::uint8_t* data, const std::size_t size,
                                  std::size_t& index) noexcept
{
    // Ensure we're not already past the available data
    if (index >= size)
    {
        return false;
    }

    const char* str_begin = reinterpret_cast<const char*>(data + index);
    phi::size_t str_len   = 0u;

    while (index < size && data[index] != '\0')
    {
        ++index;
        ++str_len;
    }

    // Reject too long strings
    if (str_len > MaxStringLength)
    {
        return false;
    }

    PHI_ASSERT(index <= size);
    // Reject strings that are not null terminated
    if (data[index - 1u] != '\0')
    {
        return false;
    }

    // Move back to proper alignment
    index += (sizeof(void*) - (index % sizeof(void*)));

    // Assign string value to cache
    cache.string = str_begin;

    return true;
}

// Returns an index into cache.vector_string
[[nodiscard]] phi::optional<phi::size_t> consume_vector_string(const std::uint8_t* data,
                                                               const std::size_t   size,
                                                               std::size_t&        index) noexcept
{
    GET_T(phi::size_t, number_of_lines);

    if (number_of_lines >= MaxVectorSize)
    {
        return {};
    }

    std::vector<std::string>& res = cache.vector_string[number_of_lines];
    for (std::size_t i{0u}; i < number_of_lines; ++i)
    {
        if (!consume_string(data, size, index))
        {
            return {};
        }

        res[i] = cache.string;
    }

    return number_of_lines;
}

template <typename T>
[[nodiscard]] std::string print_int(const T val) noexcept
{
    return fmt::format(
            "{0:d} 0x{1:02X}", phi::to_unsafe(val),
            static_cast<phi::make_unsigned_t<phi::make_unsafe_t<T>>>(phi::to_unsafe(val)));
}

template <typename T>
[[nodiscard]] std::string pretty_char(const T c) noexcept
{
    switch (c)
    {
        case '\n':
            return "\\n";
        case '\0':
            return "\\0";
        case '\t':
            return "\\t";
        case '\r':
            return "\\r";
        case '\a':
            return "\\a";
        case '\v':
            return "\\v";
        case '"':
            return "\\\"";
        case '\b':
            return "\\b";
        case '\f':
            return "\\f";

        default:
            return {1u, static_cast<const char>(c)};
    }
}

[[nodiscard]] std::string print_string(const std::string& str) noexcept
{
    std::string hex_str;
    std::string print_str;

    for (char character : str)
    {
        hex_str += fmt::format("\\0x{:02X}, ", static_cast<std::uint8_t>(character));

        // Make some special characters printable
        print_str += pretty_char(character);
    }

    return fmt::format("String(\"{:s}\" size: {:d} ({:s}))", print_str, str.size(),
                       hex_str.substr(0, hex_str.size() - 2));
}

[[nodiscard]] std::string print_char(const ImWchar character) noexcept
{
    return fmt::format(R"(ImWchar("{:s}" (\0x{:02X})))", pretty_char(character),
                       static_cast<std::uint32_t>(character));
}

[[nodiscard]] std::string print_vector_string(const std::vector<std::string>& vec) noexcept
{
    std::string ret;

    ret += fmt::format("Vector(size: {:d}):\n", vec.size());

    for (const std::string& str : vec)
    {
        ret += print_string(str) + '\n';
    }

    return ret;
}

[[nodiscard]] std::string print_error_markers(
        const dlxemu::CodeEditor::ErrorMarkers& markers) noexcept
{
    std::string ret;

    ret += fmt::format("ErrorMarkers(size: {:d}):\n", markers.size());

    for (const auto& val : markers)
    {
        ret += fmt::format("{:s}: {:s}\n", print_int(val.first), print_string(val.second));
    }

    return ret;
}

[[nodiscard]] std::string print_breakpoints(
        const dlxemu::CodeEditor::Breakpoints& breakpoints) noexcept
{
    std::string lines;

    for (const phi::u32 line_number : breakpoints)
    {
        lines += fmt::format("{:s}, ", print_int(line_number));
    }

    std::string ret = fmt::format("Breakpoints(size: {:d}: {:s})", breakpoints.size(),
                                  lines.substr(0, lines.size() - 2));

    return ret.substr(0, ret.size());
}

[[nodiscard]] const char* print_bool(const phi::boolean boolean) noexcept
{
    return boolean ? "true" : "false";
}

[[nodiscard]] phi::boolean IsReservedKey(ImGuiKey key) noexcept
{
    switch (key)
    {
        case ImGuiKey_ReservedForModCtrl:
        case ImGuiKey_ReservedForModShift:
        case ImGuiKey_ReservedForModAlt:
        case ImGuiKey_ReservedForModSuper:
            return true;

        default:
            return false;
    }
}

bool SetupImGui() noexcept
{
    IMGUI_CHECKVERSION();
    if (GImGui != nullptr)
    {
        return true;
    }

    if (ImGui::CreateContext() == nullptr)
    {
        FUZZ_LOG("Failed to create ImGuiContext");
        return false;
    }

    // Set config
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // Enforce valid display size
    io.DisplaySize.x = 1024.0f;
    io.DisplaySize.y = 768.0f;

    // Enfore valid DeltaTime
    io.DeltaTime = 1.0f / 60.0f;

    // Don't save any config
    io.IniFilename = nullptr;

    // SetStyle
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Build atlas
    unsigned char* tex_pixels{nullptr};
    int            tex_w;
    int            tex_h;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

    return true;
}

void EndImGui() noexcept
{
    ImGui::Render();

    volatile ImDrawData* draw_data = ImGui::GetDrawData();
    PHI_UNUSED_VARIABLE(draw_data);

    ImGuiIO& io = ImGui::GetIO();
    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    ImGui::EndFrame();

    // Ensure frame count doesn't overflow
    GImGui->FrameCount %= 16384;
}

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    static bool imgui_init = SetupImGui();
    (void)imgui_init;

    // Reset some ImGui states
    ImGui::GetIO().ClearInputCharacters();
    ImGui::GetIO().ClearInputKeys();
    ImGui::GetIO().InputQueueSurrogate = 0;
    GImGui->InputEventsQueue.resize(0u);
    ImGui::FocusWindow(nullptr);

    dlxemu::Emulator   emulator;
    dlxemu::CodeEditor editor{&emulator};

    editor.UpdatePalette();

    FUZZ_LOG("Beginning execution");

    for (std::size_t index{0u}; index < size;)
    {
        GET_T(phi::uint32_t, function_index);

        switch (function_index)
        {
            // AddErrorMarker
            case 0: {
                GET_T(phi::uint32_t, line_number);

                if (!consume_string(data, size, index))
                {
                    return 0;
                }
                std::string& message = cache.string;

                FUZZ_LOG("AddErrorMarker({:s}, {:s})", print_int(line_number),
                         print_string(message));

                editor.AddErrorMarker(line_number, message);
                break;
            }

            // ClearErrorMarkers
            case 1: {
                FUZZ_LOG("ClearErrorMarkers");

                editor.ClearErrorMarkers();
                break;
            }

            // SetText
            case 2: {
                if (!consume_string(data, size, index))
                {
                    return 0;
                }
                std::string& text = cache.string;

                FUZZ_LOG("SetText({:s})", print_string(text));

                editor.SetText(text);
                break;
            }

            // GetText
            case 3: {
                FUZZ_LOG("GetText()");

                volatile std::string str = editor.GetText();
                PHI_UNUSED_VARIABLE(str);
                break;
            }

            // SetTextLines
            case 4: {
                auto lines_opt = consume_vector_string(data, size, index);
                if (!lines_opt)
                {
                    return 0;
                }

                const std::vector<std::string>& lines = cache.vector_string[lines_opt.value()];

                FUZZ_LOG("SetTextLines({:s})", print_vector_string(lines));
                editor.SetTextLines(lines);
                break;
            }

            // GetTextLines
            case 5: {
                FUZZ_LOG("GetTextLines()");

                volatile std::vector<std::string> lines = editor.GetTextLines();
                PHI_UNUSED_VARIABLE(lines);
                break;
            }

            // GetSelectedText
            case 6: {
                FUZZ_LOG("GetSelectedText()");

                volatile std::string line = editor.GetSelectedText();
                PHI_UNUSED_VARIABLE(line);
                break;
            }

            // GetCurrentLineText
            case 7: {
                FUZZ_LOG("GetCurrentLineText()");

                volatile std::string line = editor.GetCurrentLineText();
                PHI_UNUSED_VARIABLE(line);
                break;
            }

            // SetReadOnly
            case 8: {
                GET_T(bool, read_only);

                FUZZ_LOG("SetReadOnly({:s})", print_bool(read_only));
                editor.SetReadOnly(read_only);
                break;
            }

            // GetCursorPosition
            case 9: {
                FUZZ_LOG("GetCursorPosition()");

                volatile dlxemu::CodeEditor::Coordinates coords = editor.GetCursorPosition();
                PHI_UNUSED_VARIABLE(coords);
                break;
            }

            // SetCursorPosition
            case 10: {
                GET_T(dlxemu::CodeEditor::Coordinates, coords);

                FUZZ_LOG("SetCursorPosition(Coordinates({:s}, {:s}))", print_int(coords.m_Line),
                         print_int(coords.m_Column));
                editor.SetCursorPosition(coords);
                break;
            }

            // SetShowWhitespaces
            case 11: {
                GET_T(bool, show_whitespaces);

                FUZZ_LOG("SetShowShitespaces({:s})", print_bool(show_whitespaces));
                editor.SetShowWhitespaces(show_whitespaces);
                break;
            }

            // SetTabSize
            case 12: {
                GET_T(std::uint_fast8_t, tab_size);

                FUZZ_LOG("SetTabSize({:s})", print_int(tab_size));
                editor.SetTabSize(tab_size);
                break;
            }

            // InsertText
            case 13: {
                if (!consume_string(data, size, index))
                {
                    return 0;
                }

                std::string& message = cache.string;

                FUZZ_LOG("InsertText({:s})", print_string(message));
                editor.InsertText(message);
                break;
            }

            // MoveUp
            case 14: {
                GET_T(phi::uint32_t, amount);
                GET_T(bool, select);

                FUZZ_LOG("MoveUp({:s}, {:s})", print_int(amount), print_bool(select));
                editor.MoveUp(amount, select);
                break;
            }

            // MoveDown
            case 15: {
                GET_T(phi::uint32_t, amount);
                GET_T(bool, select);

                FUZZ_LOG("MoveDown({:s}, {:s})", print_int(amount), print_bool(select));
                editor.MoveDown(amount, select);
                break;
            }

            // MoveLeft
            case 16: {
                GET_T(phi::uint32_t, amount);
                GET_T(bool, select);
                GET_T(bool, word_mode);

                FUZZ_LOG("MoveLeft({:s}, {:s}, {:s})", print_int(amount), print_bool(select),
                         print_bool(word_mode));
                editor.MoveLeft(amount, select, word_mode);
                break;
            }

            // MoveRight
            case 17: {
                GET_T(phi::uint32_t, amount);
                GET_T(bool, select);
                GET_T(bool, word_mode);

                FUZZ_LOG("MoveRight({:s}, {:s}, {:s})", print_int(amount), print_bool(select),
                         print_bool(word_mode));

                editor.MoveRight(amount, select, word_mode);
                break;
            }

            // MoveTop
            case 18: {
                GET_T(bool, select);

                FUZZ_LOG("MoveTop({:s})", print_bool(select));
                editor.MoveTop(select);
                break;
            }

            // MoveBottom
            case 19: {
                GET_T(bool, select);

                FUZZ_LOG("MoveBottom({:s})", print_bool(select));
                editor.MoveBottom(select);
                break;
            }

            // MoveHome
            case 20: {
                GET_T(bool, select);

                FUZZ_LOG("MoveHome({:s})", print_bool(select));
                editor.MoveHome(select);
                break;
            }

            // MoveEnd
            case 21: {
                GET_T(bool, select);

                FUZZ_LOG("MoveEnd({:s})", print_bool(select));
                editor.MoveEnd(select);
                break;
            }

            // SetSelectionStart
            case 22: {
                GET_T(dlxemu::CodeEditor::Coordinates, coords);

                FUZZ_LOG("SetSelectionStart(Coordinates({:s}, {:s}))", print_int(coords.m_Line),
                         print_int(coords.m_Column));
                editor.SetSelectionStart(coords);
                break;
            }

            // SetSelectionEnd
            case 23: {
                GET_T(dlxemu::CodeEditor::Coordinates, coords);

                FUZZ_LOG("SetSelectionEnd(Coordinates({:s}, {:s}))", print_int(coords.m_Line),
                         print_int(coords.m_Column));
                editor.SetSelectionEnd(coords);
                break;
            }

            // SetSelection
            case 24: {
                GET_T(dlxemu::CodeEditor::Coordinates, coords_start);
                GET_T(dlxemu::CodeEditor::Coordinates, coords_end);
                GET_T_COND(dlxemu::CodeEditor::SelectionMode, selection_mode,
                           selection_mode >= dlxemu::CodeEditor::SelectionMode::Normal &&
                                   selection_mode <= dlxemu::CodeEditor::SelectionMode::Line);

                FUZZ_LOG("SetSelection(Coordinates({:s}, {:s}), Coordinates({:s}, "
                         "{:s}), {:s})",
                         print_int(coords_start.m_Line), print_int(coords_start.m_Column),
                         print_int(coords_end.m_Line), print_int(coords_start.m_Column),
                         dlx::enum_name(selection_mode).data());
                editor.SetSelection(coords_start, coords_end, selection_mode);
                break;
            }

            // SelectWordUnderCursor
            case 25: {
                FUZZ_LOG("SelectWordUnderCursor");

                editor.SelectWordUnderCursor();
                break;
            }

            // SelectAll
            case 26: {
                FUZZ_LOG("SelectAll");

                editor.SelectAll();
                break;
            }

            // Delete
            case 27: {
                FUZZ_LOG("Delete");

                editor.Delete();
                break;
            }

            // Undo
            case 28: {
                FUZZ_LOG("Undo()");

                editor.Undo();
                break;
            }

            // Redo
            case 29: {
                FUZZ_LOG("Redo()");

                editor.Redo();
                break;
            }

            // SetErrorMarkers
            case 30: {
                GET_T_COND(phi::size_t, count, count <= MaxVectorSize);

                dlxemu::CodeEditor::ErrorMarkers markers;
                for (std::size_t i{0u}; i < count; ++i)
                {
                    GET_T(phi::uint32_t, line_number);

                    if (!consume_string(data, size, index))
                    {
                        return 0;
                    }
                    std::string& message = cache.string;

                    // Add to error markers
                    markers[line_number] = message;
                }

                FUZZ_LOG("SetErrorMarkers({:s})", print_error_markers(markers));

                editor.SetErrorMarkers(markers);
                break;
            }

            // SetBreakpoints
            case 31: {
                GET_T_COND(phi::size_t, count, count <= MaxVectorSize);

                dlxemu::CodeEditor::Breakpoints breakpoints;
                for (std::size_t i{0u}; i < count; ++i)
                {
                    GET_T(phi::uint32_t, line_number);

                    breakpoints.insert(line_number);
                }

                FUZZ_LOG("SetBreakpoints({:s})", print_breakpoints(breakpoints));

                editor.SetBreakpoints(breakpoints);
                break;
            }

            // Render
            case 32: {
                GET_T_COND(float, x, x <= MaxSaneFloatValue && x >= 0.0f && !phi::is_nan(x));
                GET_T_COND(float, y, y <= MaxSaneFloatValue && y >= 0.0f && !phi::is_nan(y));

                ImVec2 size_vec(x, y);

                GET_T(bool, border);

                FUZZ_LOG("Render(ImVec2({:f}, {:f}), {:s})", x, y, border ? "true" : "false");

                ImGui::NewFrame();
                editor.Render(size_vec, border);
                EndImGui();

                break;
            }

            // EnterCharacter
            case 33: {
                GET_T(ImWchar, character);
                GET_T(bool, shift);

                FUZZ_LOG("EnterCharacter({:s}, {:s})", print_char(character), print_bool(shift));
                editor.EnterCharacter(character, shift);
                break;
            }

            // ClearText
            case 34: {
                FUZZ_LOG("ClearText()");

                editor.ClearText();
                break;
            }

            // ClearSelection
            case 35: {
                FUZZ_LOG("ClearSelection");

                editor.ClearSelection();
                break;
            }

            // Backspace
            case 36: {
                FUZZ_LOG("Backspace");

                editor.Backspace();
                break;
            }

            // ImGui::AddKeyEvent
            case 37: {
                GET_T_COND(ImGuiKey, key,
                           ImGui::IsNamedKey(key) && !ImGui::IsAliasKey(key) &&
                                   !IsReservedKey(key));
                GET_T(bool, down);

                FUZZ_LOG("ImGui::GetIO().AddKeyEvent({}, {:s})", key, print_bool(down));
                ImGui::GetIO().AddKeyEvent(key, down);

                break;
            }

            // ImGui::AddKeyAnalogEvent
            case 38: {
                GET_T_COND(ImGuiKey, key,
                           ImGui::IsNamedKey(key) && !ImGui::IsAliasKey(key) &&
                                   !IsReservedKey(key));
                GET_T(bool, down);
                GET_T_COND(float, value, phi::abs(value) <= MaxSaneFloatValue);

                FUZZ_LOG("ImGui::GetIO().AddKeyAnalogEvent({}, {:s}, {:f})", key, print_bool(down),
                         value);
                ImGui::GetIO().AddKeyAnalogEvent(key, down, value);

                break;
            }

            // ImGui::AddMousePosEvent
            case 39: {
                GET_T_COND(float, x, phi::abs(x) <= MaxSaneFloatValue);
                GET_T_COND(float, y, phi::abs(y) <= MaxSaneFloatValue);

                FUZZ_LOG("ImGui::GetIO().AddMousePosEvent({:f}, {:f})", x, y);
                ImGui::GetIO().AddMousePosEvent(x, y);

                break;
            }

            // ImGui::AddMouseButtonEvent
            case 40: {
                GET_T_COND(int, button, button >= 0 && button < ImGuiMouseButton_COUNT);
                GET_T(bool, down);

                FUZZ_LOG("ImGui::GetIO().AddMouseButtonEvent({}, {:s})", button, print_bool(down));
                ImGui::GetIO().AddMouseButtonEvent(button, down);

                break;
            }

            // ImGui::AddMouseWheelEvent
            case 41: {
                GET_T_COND(float, wh_x, phi::abs(wh_x) <= MaxSaneFloatValue);
                GET_T_COND(float, wh_y, phi::abs(wh_y) <= MaxSaneFloatValue);

                FUZZ_LOG("ImGui::GetIO().AddMouseWheelEvent({:f}, {:f})", wh_x, wh_y);
                ImGui::GetIO().AddMouseWheelEvent(wh_x, wh_y);

                break;
            }

            // ImGui::AddFocusEvent
            case 42: {
                GET_T(bool, focused);

                FUZZ_LOG("ImGui::GetIO().AddFocusEvent({:s})", print_bool(focused));
                ImGui::GetIO().AddFocusEvent(focused);

                break;
            }

            // ImGui::AddInputCharacter
            case 43: {
                GET_T(unsigned int, character);

                FUZZ_LOG("ImGui::GetIO().AddInputCharacter({})", character);
                ImGui::GetIO().AddInputCharacter(character);

                break;
            }

            // ImGui::AddInputCharacterUTF16
            case 44: {
                GET_T(ImWchar16, character);

                FUZZ_LOG("ImGui::GetIO().AddInputCharacterUTF16({})", character);
                ImGui::GetIO().AddInputCharacterUTF16(character);

                break;
            }

            // ImGui::AddInputCharactersUTF8
            case 45: {
                if (!consume_string(data, size, index))
                {
                    return 0;
                }
                std::string& str = cache.string;

                FUZZ_LOG("ImGui::GetIO().AddInputCharactersUTF8({:s})", print_string(str));
                ImGui::GetIO().AddInputCharactersUTF8(str.c_str());

                break;
            }

            // Copy
            case 46: {
                FUZZ_LOG("Copy()");
                editor.Copy();

                break;
            }

            // Cut
            case 47: {
                FUZZ_LOG("Cut()");
                editor.Cut();

                break;
            }

            // Paste
            case 48: {
                FUZZ_LOG("Paste()");
                editor.Paste();

                break;
            }

            // SetOverwrite
            case 49: {
                GET_T(bool, overwrite);

                FUZZ_LOG("SetOverwrite({:s})", print_bool(overwrite));
                editor.SetOverwrite(overwrite);

                break;
            }

            // SetColorizerEnable
            case 50: {
                GET_T(bool, colorizer);

                FUZZ_LOG("SetColorizerEnable({:s})", print_bool(colorizer));
                editor.SetColorizerEnable(colorizer);

                break;
            }

            // RemoveBreakpoint
            case 51: {
                GET_T(phi::u32, line_number);

                FUZZ_LOG("RemoveBreakpoint({:s})", print_int(line_number));
                editor.RemoveBreakpoint(line_number);

                break;
            }

            // ToggleBreakpoint
            case 52: {
                GET_T(phi::u32, line_number);

                FUZZ_LOG("ToggleBreakpoint({:s})", print_int(line_number));
                editor.ToggleBreakpoint(line_number);

                break;
            }
        }
    }

    FUZZ_LOG("VerifyInternalState()");
    editor.VerifyInternalState();

    FUZZ_LOG("Finished execution");

    return 0;
}
