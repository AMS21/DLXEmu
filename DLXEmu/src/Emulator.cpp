#include "DLXEmu/Emulator.hpp"

#include "DLXEmu/BuildInfo.hpp"
#include <Phi/Config/Compiler.hpp>
#include <Phi/Config/Platform.hpp>
#include <Phi/Core/Log.hpp>
#include <imgui.h>

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
        if (m_ShowAbout)
        {
            RenderAbout();
        }

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
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Quit", "Alt+F4"))
                {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z"))
                {}

                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
                {} // Disabled item

                ImGui::Separator();

                if (ImGui::MenuItem("Cut", "CTRL+X"))
                {}

                if (ImGui::MenuItem("Copy", "CTRL+C"))
                {}

                if (ImGui::MenuItem("Paste", "CTRL+V"))
                {}

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                ImGui::MenuItem("Control Panel", "", &m_ShowControlPanel);

                ImGui::MenuItem("Memory Viewer", "", &m_ShowMemoryViewer);

                ImGui::MenuItem("Registry Viewer", "", &m_ShowRegisterViewer);

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
        if (ImGui::Begin("Control Panel"), &m_ShowControlPanel)
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
            ImGui::Text("Version: %d.%d.%d", dlxemu::VersionMajor, dlxemu::VersionMinor,
                        dlxemu::VersionPatch);
            ImGui::Text("Commit: %s", dlxemu::GitShaFull);
            ImGui::Text("Build date: %s %s", dlxemu::BuildDate, dlxemu::BuildTime);
            ImGui::Text("OS: %s", PHI_PLATFORM_NAME());
            ImGui::Text("Compiler: %s (%d.%d.%d)", PHI_COMPILER_NAME(),
                        PHI_CURRENT_COMPILER_VERSION_MAJOR(), PHI_CURRENT_COMPILER_VERSION_MINOR(),
                        PHI_CURRENT_COMPILER_VERSION_PATCH());

            if (ImGui::Button("OK"))
            {
                m_ShowAbout = false;
            }
        }

        ImGui::End();
    }
} // namespace dlxemu