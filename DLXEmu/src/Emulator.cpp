#include "DLXEmu/Emulator.hpp"

#include <glad/glad.h>

#include "DLXEmu/generated/BuildInfo.hpp"
#include <DLX/TokenStream.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <phi/algorithm/string_length.hpp>
#include <phi/compiler_support/compiler.hpp>
#include <phi/compiler_support/platform.hpp>
#include <phi/core/assert.hpp>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <string_view>

namespace dlxemu
{
    Emulator::Emulator() noexcept
        : m_CodeEditor(this)
        , m_MemoryViewer(this)
        , m_RegisterViewer(this)
#if defined(PHI_DEBUG)
        , m_DebugView(this)
#endif
    {}

    phi::boolean Emulator::HandleCommandLineArguments(phi::i32 argc, char** argv) noexcept
    {
        // No args
        if (argc <= 1)
        {
            SPDLOG_DEBUG("No args provides");
            return true;
        }

        for (phi::i32 arg_num{1}; arg_num < argc; ++arg_num)
        {
            std::string arg_value = argv[arg_num.get()];
            std::transform(arg_value.begin(), arg_value.end(), arg_value.begin(), ::tolower);

            PHI_ASSERT(!arg_value.empty());

            if (arg_value.front() == '-')
            {
                // Display help
                if (arg_value == "-h" || arg_value == "-help" || arg_value == "-?" ||
                    arg_value == "--help")
                {
                    SPDLOG_INFO("Help");
                    return false;
                }
                // Display version
                if (arg_value == "-v" || arg_value == "--value")
                {
                    fmt::print("DLXEmu version {:d}.{:d}.{:d} {:s}-{:s}\n", VersionMajor,
                               VersionMinor, VersionPatch, GitBranch, GitShaFull);
                    return false;
                }

                // Unknown option
                SPDLOG_WARN("Unknown option '{:s}'", arg_value);
                break;
            }

            SPDLOG_WARN("Ignore command line argument '{:s}'", arg_value);
        }

        return true;
    }

    phi::boolean Emulator::Initialize() noexcept
    {
        // Initialize Window
        if (!m_Window.Initialize())
        {
            SPDLOG_ERROR("Failed to initialize window");
            return false;
        }

        return true;
    }

    phi::boolean Emulator::IsRunning() const noexcept
    {
        return m_Window.IsOpen();
    }

    void Emulator::MainLoop() noexcept
    {
        m_Window.BeginFrame();

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

    dlx::Processor& Emulator::GetProcessor() noexcept
    {
        return m_Processor;
    }

    const dlx::ParsedProgram& Emulator::GetProgram() const noexcept
    {
        return m_DLXProgram;
    }

    void Emulator::ParseProgram(std::string_view source) noexcept
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
                {}

#if PHI_PLATFORM_IS_NOT(WEB)
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {}
#endif

                if (ImGui::MenuItem("Save As..", "Ctrl+Shift+S"))
                {}

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
                if (ImGui::MenuItem("Undo", "CTRL+Z", false, m_CodeEditor.CanUndo()))
                {
                    m_CodeEditor.Undo();
                }

                if (ImGui::MenuItem("Redo", "CTRL+Y", false, m_CodeEditor.CanRedo()))
                {
                    m_CodeEditor.Redo();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Cut", "CTRL+X", false, m_CodeEditor.HasSelection()))
                {
                    m_CodeEditor.Cut();
                }

                if (ImGui::MenuItem("Copy", "CTRL+C", false, m_CodeEditor.HasSelection()))
                {
                    m_CodeEditor.Copy();
                }

                bool can_paste = std::strcmp(ImGui::GetClipboardText(), "") != 0;

                if (ImGui::MenuItem("Paste", "CTRL+V", false, can_paste))
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
                    SPDLOG_TRACE("Register dump:\n" + m_Processor.GetRegisterDump());
                }

                if (ImGui::MenuItem("Dump memory to console"))
                {
                    SPDLOG_TRACE("Memory dump:\n" + m_Processor.GetMemoryDump());
                }

                if (ImGui::MenuItem("Dump processor to console"))
                {
                    SPDLOG_TRACE("Processor dump:\n" + m_Processor.GetProcessorDump());
                }

                if (ImGui::MenuItem("Dump current program to console"))
                {
                    SPDLOG_TRACE("Current program dump:\n" + m_DLXProgram.GetDump());
                }

                if (ImGui::MenuItem("Full console dump"))
                {
                    SPDLOG_TRACE("Register dump:\n" + m_Processor.GetRegisterDump());
                    SPDLOG_TRACE("Memory dump:\n" + m_Processor.GetMemoryDump());
                    SPDLOG_TRACE("Processor dump:\n" + m_Processor.GetProcessorDump());
                    SPDLOG_TRACE("Current program dump:\n" + m_DLXProgram.GetDump());
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
            if (ImGui::Button("R"))
            {
                // Run
                if (m_DLXProgram.m_ParseErrors.empty())
                {
                    m_Processor.ExecuteCurrentProgram();
                    SPDLOG_INFO("Executed current program");
                }
                else
                {
                    SPDLOG_INFO("Can't execute program since it contains {} parse errors",
                                m_DLXProgram.m_ParseErrors.size());
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("S"))
            {
                // Step
                if (m_Processor.GetCurrentStepCount() == 0u)
                {
                    SPDLOG_INFO("Loaded program");
                    m_Processor.LoadProgram(m_DLXProgram);
                }

                m_Processor.ExecuteStep();

                SPDLOG_INFO("Executed step");
            }

            ImGui::SameLine();
            if (ImGui::Button("D"))
            {
                // Discard? / Reset
                m_Processor.LoadProgram(m_DLXProgram);
            }
        }

        ImGui::End();
    }

    constexpr static const char* get_lsb_info() noexcept
    {
        if (phi::string_length(dlxemu::LSBId) == 0u)
        {
            return "";
        }

        return "\nLSB:        " DLXEMU_LSB_DESCRIPTION " - " DLXEMU_LSB_RELEASE
               " " DLXEMU_LSB_CODENAME;
    }

    void Emulator::RenderAbout() noexcept
    {
        constexpr static ImGuiWindowFlags about_flags =
                ImGuiWindowFlags_AlwaysAutoResize + ImGuiWindowFlags_NoDocking +
                ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse +
                ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("DLXEmu - About", &m_ShowAbout, about_flags))
        {
            static constexpr const char* arch_flag{sizeof(void*) == 8 ? "x64" : "x32"};
            static constexpr const char* lsb_info{get_lsb_info()};

            static std::string about_text = fmt::format(
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
                    GLVersion.major, GLVersion.minor, GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR,
                    GLFW_VERSION_REVISION, IMGUI_VERSION, PHI_PLATFORM_NAME(), arch_flag,
                    DLXEMU_UNAME, PHI_COMPILER_NAME(), PHI_CURRENT_COMPILER_VERSION_MAJOR(),
                    PHI_CURRENT_COMPILER_VERSION_MINOR(), PHI_CURRENT_COMPILER_VERSION_PATCH(),
                    lsb_info);

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

    void Emulator::RenderOptionsMenu() noexcept
    {
        constexpr static ImGuiWindowFlags options_flags =
                ImGuiWindowFlags_NoDocking + ImGuiWindowFlags_NoCollapse;

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
                                break;
                            case 1:
                                ImGui::StyleColorsLight();
                                break;
                            case 2:
                                ImGui::StyleColorsClassic();
                                break;
                            default:
                                PHI_ASSERT_NOT_REACHED();
                        }
                    }

                    static int palete_idx = 0;
                    if (ImGui::Combo("Palete", &palete_idx, "Dark\0Light\0Retro\0"))
                    {
                        switch (palete_idx)
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
} // namespace dlxemu
