#include "DLXEmu/Emulator.hpp"

#include <glad/gl.h>

#include <DLX/Logger.hpp>
#include <DLX/TokenStream.hpp>
#include <DLXEmu/generated/BuildInfo.hpp>
#include <DLXEmu/generated/ThirdPartyLicense.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <phi/algorithm/for_each.hpp>
#include <phi/algorithm/string_length.hpp>
#include <phi/compiler_support/compiler.hpp>
#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/platform.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>
#include <phi/text/to_lower_case.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_GCC_SUPPRESS_WARNING_POP()

static constexpr const phi::size_t MaxExecutionPerFrame{500'000u};

static int glad_gl_version;

namespace dlxemu
{
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4355)

    Emulator::Emulator() noexcept
        : m_CodeEditor(this)
        , m_MemoryViewer(this)
        , m_RegisterViewer(this)
#if defined(PHI_DEBUG)
        , m_DebugView(this)
#endif
    {}

    PHI_MSVC_SUPPRESS_WARNING_POP()

    PHI_CLANG_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")

    Emulator::ShouldContinueInitialization Emulator::HandleCommandLineArguments(
            phi::i32 argc, char** argv) noexcept
    {
        // No args
        if (argc <= 1)
        {
            return ShouldContinueInitialization::Yes;
        }

        for (phi::i32 arg_num{1}; arg_num < argc; ++arg_num)
        {
            std::string arg_value = argv[arg_num.unsafe()];

            // Convert all characters to lower case
            phi::for_each(arg_value.begin(), arg_value.end(), phi::to_lower_case);
            //std::transform(arg_value.begin(), arg_value.end(), arg_value.begin(), ::tolower);

            PHI_ASSERT(!arg_value.empty());

            if (arg_value.front() == '-')
            {
                // Display help
                if (arg_value == "-h" || arg_value == "-help" || arg_value == "-?" ||
                    arg_value == "--help")
                {
                    DLX_INFO("Help");
                    return ShouldContinueInitialization::No;
                }
                // Display version
                if (arg_value == "-v" || arg_value == "--value")
                {
                    fmt::print("DLXEmu version {:d}.{:d}.{:d} {:s}-{:s}\n", VersionMajor,
                               VersionMinor, VersionPatch, GitBranch, GitShaFull);
                    return ShouldContinueInitialization::No;
                }

                // Unknown option
                DLX_WARN("Unknown option '{:s}' ignored", arg_value);
                break;
            }

            DLX_WARN("Ignore command line argument '{:s}'", arg_value);
        }

        return ShouldContinueInitialization::Yes;
    }

    PHI_CLANG_SUPPRESS_WARNING_POP()

    phi::boolean Emulator::Initialize() noexcept
    {
        // Initialize Window
        if (!m_Window.Initialize())
        {
            DLX_ERROR("Failed to initialize window");
            return false;
        }

        // Make sure the palette is updated correctly
        m_CodeEditor.UpdatePalette();

        return true;
    }

    phi::boolean Emulator::IsRunning() const noexcept
    {
        return m_Window.IsOpen();
    }

    void Emulator::MainLoop() noexcept
    {
        m_Window.BeginFrame();

        m_DisableEditing = m_CurrentExecutionMode != ExecutionMode::None &&
                           m_CurrentExecutionMode != ExecutionMode::SingleStep;

        // Run updates
        Update();

        // Add docking space
        const auto* viewport = ImGui::GetWindowViewport();
        ImGui::DockSpaceOverViewport(viewport);

        // Render our stuff
        RenderMenuBar();
        m_CodeEditor.Render();

        if (m_ShowControlPanel)
        {
            RenderControlPanel();
        }
        if (m_ShowMemoryViewer)
        {
            m_MemoryViewer.Render();
        }
        if (m_ShowRegisterViewer)
        {
            m_RegisterViewer.Render();
        }
        if (m_ShowOptionsMenu)
        {
            RenderOptionsMenu();
        }
        if (m_ShowAbout)
        {
            RenderAbout();
        }
        if (m_ShowThirdPartyLicense)
        {
            RenderThirdPartyLicense();
        }

#if defined(PHI_DEBUG)
        if (m_ShowDebugView)
        {
            m_DebugView.Render();
        }

        if (m_ShowDemoWindow)
        {
            ImGui::ShowDemoWindow(&m_ShowDemoWindow);
        }
#endif

        m_Window.EndFrame();
    }

    PHI_ATTRIBUTE_CONST dlx::Processor& Emulator::GetProcessor() noexcept
    {
        return m_Processor;
    }

    PHI_ATTRIBUTE_CONST const dlx::ParsedProgram& Emulator::GetProgram() const noexcept
    {
        return m_DLXProgram;
    }

    void Emulator::ParseProgram(phi::string_view source) noexcept
    {
        m_DLXProgram = dlx::Parser::Parse(source);

        if (m_DLXProgram.m_ParseErrors.empty())
        {
            m_Processor.LoadProgram(m_DLXProgram);
        }
    }

    void Emulator::ParseProgram(dlx::TokenStream& tokens) noexcept
    {
        m_DLXProgram = dlx::Parser::Parse(tokens);

        if (m_DLXProgram.m_ParseErrors.empty())
        {
            m_Processor.LoadProgram(m_DLXProgram);
        }
    }

    PHI_ATTRIBUTE_CONST CodeEditor& Emulator::GetEditor() noexcept
    {
        return m_CodeEditor;
    }

    PHI_ATTRIBUTE_CONST const CodeEditor& Emulator::GetEditor() const noexcept
    {
        return m_CodeEditor;
    }

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wsuggest-attribute=pure")

    PHI_ATTRIBUTE_CONST Emulator::ExecutionMode Emulator::GetExecutionMode() const noexcept
    {
        return m_CurrentExecutionMode;
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

    PHI_ATTRIBUTE_CONST phi::u64 Emulator::GetExecutingLineNumber() const noexcept
    {
        if (m_DLXProgram.IsValid() && !m_Processor.IsHalted() &&
            m_CurrentExecutionMode != ExecutionMode::None)
        {
            PHI_ASSERT(m_Processor.GetProgramCounter() < m_DLXProgram.m_Instructions.size());

            const auto& current_instruction =
                    m_DLXProgram.m_Instructions.at(m_Processor.GetProgramCounter().unsafe());

            return current_instruction.GetSourceLine();
        }

        // Not valid
        return 0u;
    }

    void Emulator::RenderMenuBar() noexcept
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {
                    m_CodeEditor.ClearText();
                }

                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                }

#if PHI_PLATFORM_IS_NOT(WEB)
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                }
#endif

                if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
                {
                }

                ImGui::Separator();
                if (ImGui::MenuItem("Options"))
                {
                    m_ShowOptionsMenu = true;
                }

#if PHI_PLATFORM_IS_NOT(WEB)
                if (ImGui::MenuItem("Quit", "Alt+F4"))
                {
                    m_Window.Close();
                }
#endif
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z", false, m_CodeEditor.CanUndo().unsafe()))
                {
                    m_CodeEditor.Undo();
                }

                if (ImGui::MenuItem("Redo", "CTRL+Y", false, m_CodeEditor.CanRedo().unsafe()))
                {
                    m_CodeEditor.Redo();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Cut", "CTRL+X", false, m_CodeEditor.HasSelection().unsafe()))
                {
                    m_CodeEditor.Cut();
                }

                if (ImGui::MenuItem("Copy", "CTRL+C", false, m_CodeEditor.HasSelection().unsafe()))
                {
                    m_CodeEditor.Copy();
                }

                const phi::boolean can_paste =
                        phi::string_length(ImGui::GetClipboardText()) != 0u && !m_DisableEditing;

                if (ImGui::MenuItem("Paste", "CTRL+V", false, can_paste.unsafe()))
                {
                    m_CodeEditor.Paste();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Control Panel", "", &m_ShowControlPanel);
                ImGui::MenuItem("Memory Viewer", "", &m_ShowMemoryViewer);
                ImGui::MenuItem("Registry Viewer", "", &m_ShowRegisterViewer);

#if defined(PHI_DEBUG)
                ImGui::Separator();

                ImGui::MenuItem("Dear ImGui Demo Window", "", &m_ShowDemoWindow);
                ImGui::MenuItem("Debug View", "", &m_ShowDebugView);
#endif

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("Third-Party Licenses"))
                {
                    m_ShowThirdPartyLicense = true;
                }

                if (ImGui::MenuItem("About"))
                {
                    m_ShowAbout = true;
                }

                ImGui::EndMenu();
            }

#if defined(PHI_DEBUG)
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Dear ImGui Demo Window", "", &m_ShowDemoWindow);
                ImGui::MenuItem("Debug View", "", &m_ShowDebugView);

                ImGui::Separator();

                if (ImGui::MenuItem("Dump registers to console"))
                {
                    DLX_TRACE("Register dump:\n" + m_Processor.GetRegisterDump());
                }

                if (ImGui::MenuItem("Dump memory to console"))
                {
                    DLX_TRACE("Memory dump:\n" + m_Processor.GetMemoryDump());
                }

                if (ImGui::MenuItem("Dump processor to console"))
                {
                    DLX_TRACE("Processor dump:\n" + m_Processor.GetProcessorDump());
                }

                if (ImGui::MenuItem("Dump current program to console"))
                {
                    DLX_TRACE("Current program dump:\n" + m_DLXProgram.GetDump());
                }

                if (ImGui::MenuItem("Full console dump"))
                {
                    DLX_TRACE("Register dump:\n" + m_Processor.GetRegisterDump());
                    DLX_TRACE("Memory dump:\n" + m_Processor.GetMemoryDump());
                    DLX_TRACE("Processor dump:\n" + m_Processor.GetProcessorDump());
                    DLX_TRACE("Current program dump:\n" + m_DLXProgram.GetDump());
                }

                ImGui::EndMenu();
            }
#endif

            ImGui::EndMainMenuBar();
        }
    }

    void Emulator::RenderControlPanel() noexcept
    {
        if (ImGui::Begin("Control Panel", &m_ShowControlPanel))
        {
            if (!m_DLXProgram.IsValid())
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Run"))
            {
                SetExecutionMode(ExecutionMode::Run);
            }

            ImGui::SameLine();
            if (ImGui::Button("Play"))
            {
                SetExecutionMode(ExecutionMode::StepThrough);
            }

            ImGui::SameLine();
            if (ImGui::Button("Step"))
            {
                if (m_Processor.GetCurrentStepCount() == 0u)
                {
                    DLX_INFO("Loaded program");
                    m_Processor.LoadProgram(m_DLXProgram);
                }

                SetExecutionMode(ExecutionMode::SingleStep);

                m_Processor.ExecuteStep();

                DLX_INFO("Executed step");
            }

            if (!m_DLXProgram.IsValid())
            {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                SetExecutionMode(ExecutionMode::None);
                m_Processor.LoadProgram(m_DLXProgram);
            }

            // Execution details
            ImGui::SameLine();
            ImGui::Text("Halted: %s,", m_Processor.IsHalted() ? "Yes" : "No");

            ImGui::SameLine();
            ImGui::Text("PC: %u,", m_Processor.GetProgramCounter().unsafe());

            ImGui::SameLine();
            ImGui::Text("SC: %lu", m_Processor.GetCurrentStepCount().unsafe());

            ImGui::SameLine();
            if (m_DLXProgram.IsValid() && !m_Processor.IsHalted() &&
                m_CurrentExecutionMode != ExecutionMode::None)
            {
                PHI_ASSERT(m_Processor.GetProgramCounter() < m_DLXProgram.m_Instructions.size());

                const auto& current_instruction =
                        m_DLXProgram.m_Instructions.at(m_Processor.GetProgramCounter().unsafe());
#if PHI_COMPILER_IS(EMCC)
                ImGui::Text("LN: %llu", current_instruction.GetSourceLine().unsafe());
#else
                ImGui::Text("LN: %lu", current_instruction.GetSourceLine().unsafe());
#endif
            }
            else
            {
                ImGui::Text("LN: N/A");
            }
        }

        ImGui::End();
    }

    constexpr static const char* get_lsb_info() noexcept
    {
        PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunreachable-code-return")

        if (phi::string_length(dlxemu::LSBId) == 0u)
        {
            return "";
        }

        PHI_CLANG_SUPPRESS_WARNING_POP()

        return "\nLSB:        " DLXEMU_LSB_DESCRIPTION " - " DLXEMU_LSB_RELEASE
               " " DLXEMU_LSB_CODENAME;
    }

    void Emulator::RenderAbout() noexcept
    {
        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5264) // Unused variable?

        static constexpr const ImGuiWindowFlags about_flags =
                ImGuiWindowFlags_AlwaysAutoResize + ImGuiWindowFlags_NoDocking +
                ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse +
                ImGuiWindowFlags_NoSavedSettings;

        PHI_MSVC_SUPPRESS_WARNING_POP()

        if (ImGui::Begin("DLXEmu - About", &m_ShowAbout, about_flags))
        {
            static constexpr const char* arch_flag{sizeof(void*) == 8 ? "x64" : "x32"};
            static constexpr const char* lsb_info{get_lsb_info()};

            PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wexit-time-destructors")

            static const std::string about_text = fmt::format(
                    "Version:    {:d}.{:d}.{:d} {:s}\n"
                    "Commit:     {:s}\n"
                    "Build date: {:s} {:s}\n"
                    "OpenGL:     {:d}.{:d}\n"
                    "GLFW:       {:d}.{:d}.{:d}\n"
                    "Dear ImGui: {:s}\n"
                    "Platform:   {:s} {:s}\n"
                    "Uname:      {:s}\n"
                    "Compiler:   {:s} ({:d}.{:d}.{:d}){}",
                    dlxemu::VersionMajor, dlxemu::VersionMinor, dlxemu::VersionPatch,
                    dlxemu::GitBranch, dlxemu::GitShaFull, dlxemu::BuildDate, dlxemu::BuildTime,
                    GLAD_VERSION_MAJOR(glad_gl_version), GLAD_VERSION_MINOR(glad_gl_version),
                    GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION, IMGUI_VERSION,
                    PHI_PLATFORM_NAME(), arch_flag, DLXEMU_UNAME, PHI_COMPILER_NAME(),
                    PHI_CURRENT_COMPILER_VERSION_MAJOR(), PHI_CURRENT_COMPILER_VERSION_MINOR(),
                    PHI_CURRENT_COMPILER_VERSION_PATCH(), lsb_info);

            PHI_CLANG_SUPPRESS_WARNING_POP()

            ImGui::TextUnformatted(about_text.c_str());

            if (ImGui::Button("Copy"))
            {
                ImGui::SetClipboardText(about_text.c_str());
            }

            ImGui::SameLine();

            if (ImGui::Button("OK"))
            {
                m_ShowAbout = false;
            }
        }

        ImGui::End();
    } // namespace dlxemu

    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

    void Emulator::RenderOptionsMenu() noexcept
    {
        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5264)

        static constexpr const ImGuiWindowFlags options_flags =
                ImGuiWindowFlags_NoDocking + ImGuiWindowFlags_NoCollapse;

        PHI_MSVC_SUPPRESS_WARNING_POP()

        if (ImGui::Begin("Options", &m_ShowOptionsMenu, options_flags))
        {
            if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Style"))
                {
                    static int style_idx = 0;
                    if (ImGui::Combo("Color", &style_idx, "Dark\0Light\0Classic\0"))
                    {
                        switch (style_idx)
                        {
                            case 0:
                                ImGui::StyleColorsDark();
                                m_CodeEditor.UpdatePalette();
                                break;
                            case 1:
                                ImGui::StyleColorsLight();
                                m_CodeEditor.UpdatePalette();
                                break;
                            case 2:
                                ImGui::StyleColorsClassic();
                                m_CodeEditor.UpdatePalette();
                                break;
                            default:
                                PHI_ASSERT_NOT_REACHED();
                        }
                    }

                    static int palette_idx = 0;
                    if (ImGui::Combo("Palette", &palette_idx, "Dark\0Light\0Retro\0"))
                    {
                        switch (palette_idx)
                        {
                            case 0:
                                m_CodeEditor.SetPalette(dlxemu::CodeEditor::GetDarkPalette());
                                break;
                            case 1:
                                m_CodeEditor.SetPalette(dlxemu::CodeEditor::GetLightPalette());
                                break;
                            case 2:
                                m_CodeEditor.SetPalette(dlxemu::CodeEditor::GetRetroBluePalette());
                                break;
                            default:
                                PHI_ASSERT_NOT_REACHED();
                        }
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }

        ImGui::End();
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()

    PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wcovered-switch-default")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

    void Emulator::RenderThirdPartyLicense() noexcept
    {
        static constexpr const ImGuiWindowFlags third_party_flags =
                ImGuiWindowFlags_AlwaysAutoResize + ImGuiWindowFlags_NoDocking +
                ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse +
                ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("DLXEmu - Third-Party License", &m_ShowThirdPartyLicense,
                         third_party_flags))
        {
            if (ImGui::CollapsingHeader("{fmt}"))
            {
                ImGui::TextUnformatted(fmt_license);
            }

            if (ImGui::CollapsingHeader("glad"))
            {
                ImGui::TextUnformatted(glad_license);
            }

            if (ImGui::CollapsingHeader("glfw"))
            {
                ImGui::TextUnformatted(glfw_license);
            }

            if (ImGui::CollapsingHeader("imgui"))
            {
                ImGui::TextUnformatted(imgui_license);
            }

            if (ImGui::CollapsingHeader("Phi"))
            {
                ImGui::TextUnformatted(phi_license);
            }

            ImGui::End();
        }
    }

    void Emulator::Update() noexcept
    {
        switch (m_CurrentExecutionMode)
        {
            case ExecutionMode::None:
            case ExecutionMode::SingleStep: {
                return;
            }
            case ExecutionMode::StepThrough: {
                const double current_time = ImGui::GetTime();
                if (m_LastExecTime + m_StepThroughDelayMS <= current_time)
                {
                    m_Processor.ExecuteStep();
                    m_LastExecTime = current_time;
                }
                break;
            }
            case ExecutionMode::Run: {
                for (phi::usize i{0u}; i < MaxExecutionPerFrame; ++i)
                {
                    m_Processor.ExecuteStep();
                }
                break;
            }

            default:
                PHI_ASSERT_NOT_REACHED();
                break;
        }

        if (m_Processor.IsHalted())
        {
            DLX_INFO("Processor halted");
            SetExecutionMode(ExecutionMode::None);
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_CLANG_SUPPRESS_WARNING_POP()

    void Emulator::SetExecutionMode(ExecutionMode mode) noexcept
    {
        m_CurrentExecutionMode = mode;

        if (mode != ExecutionMode::None)
        {
            m_LastExecTime = ImGui::GetTime();
            m_CodeEditor.SetReadOnly(true);
        }
        else
        {
            m_CodeEditor.SetReadOnly(false);
        }
    }
} // namespace dlxemu
