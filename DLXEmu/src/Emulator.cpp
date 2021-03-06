#include "DLXEmu/Emulator.hpp"

#include "DLXEmu/BuildInfo.hpp"
#include "Phi/Core/Assert.hpp"
#include <Phi/Config/Compiler.hpp>
#include <Phi/Config/Platform.hpp>
#include <Phi/Core/Log.hpp>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>

namespace dlxemu
{
    Emulator::Emulator() noexcept
        : m_CodeEditor(this)
        , m_MemoryViewer(this)
        , m_RegisterViewer(this)
    {}

    phi::Boolean Emulator::Initialize() noexcept
    {
        // Initialize Window
        if (!m_Window.Initialize())
        {
            PHI_LOG_ERROR("Failed to initialize window");
            return false;
        }

        return true;
    }

    phi::Boolean Emulator::IsRunning() const noexcept
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

    const dlx::InstructionLibrary& Emulator::GetInstructionLibrary() const noexcept
    {
        return m_InstructionLibrary;
    }

    const dlx::ParsedProgram& Emulator::GetProgram() const noexcept
    {
        return m_DLXProgram;
    }

    void Emulator::ParseProgram(std::string_view source) noexcept
    {
        m_DLXProgram = dlx::Parser::Parse(m_InstructionLibrary, source);
    }

    void Emulator::ParseProgram(std::vector<dlx::Token> tokens) noexcept
    {
        m_DLXProgram = dlx::Parser::Parse(m_InstructionLibrary, tokens);
    }

    void Emulator::RenderMenuBar() noexcept
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New"))
                {}

                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {}

                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {}

                if (ImGui::MenuItem("Save As.."))
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
                ParseProgram(m_CodeEditor.GetText());
                if (!m_Processor.LoadProgram(m_DLXProgram))
                {
                    PHI_LOG_INFO("Can't execute program since it contains {} parse errors",
                                 m_DLXProgram.m_ParseErrors.size());
                }
                else
                {
                    m_Processor.ExecuteCurrentProgram();
                    PHI_LOG_INFO("Executed current program");
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("S"))
            {
                // Step
                if (m_Processor.GetCurrentStepCount() == 0u)
                {
                    PHI_LOG_INFO("Loaded program");
                    ParseProgram(m_CodeEditor.GetText());
                    m_Processor.LoadProgram(m_DLXProgram);
                }

                m_Processor.ExecuteStep();

                PHI_LOG_INFO("Executed step");
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

    void Emulator::RenderAbout() noexcept
    {
        constexpr static ImGuiWindowFlags about_flags =
                ImGuiWindowFlags_AlwaysAutoResize + ImGuiWindowFlags_NoDocking +
                ImGuiWindowFlags_NoResize + ImGuiWindowFlags_NoCollapse +
                ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("DLXEmu - About", &m_ShowAbout, about_flags))
        {
            static std::string about_text = fmt::format(
                    "Version: {}.{}.{}\n"
                    "Commit: {}\n"
                    "Build date: {} {}\n"
                    "Platform: {}\n"
                    "Compiler: {} ({}.{}.{})",
                    dlxemu::VersionMajor, dlxemu::VersionMinor, dlxemu::VersionPatch,
                    dlxemu::GitShaFull, dlxemu::BuildDate, dlxemu::BuildTime, PHI_PLATFORM_NAME(),
                    PHI_COMPILER_NAME(), PHI_CURRENT_COMPILER_VERSION_MAJOR(),
                    PHI_CURRENT_COMPILER_VERSION_MINOR(), PHI_CURRENT_COMPILER_VERSION_PATCH());

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
    }

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
