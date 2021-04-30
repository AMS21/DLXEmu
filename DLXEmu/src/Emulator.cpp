#include "DLXEmu/Emulator.hpp"

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
        RenderControlPanel();
        m_CodeEditor.Render();
        m_MemoryViewer.Render();
        m_RegisterViewer.Render();

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

    void Emulator::RenderControlPanel() noexcept
    {
        ImGui::Begin("Control Panel");

        if (ImGui::Button("R"))
        {
            // Run;
            ParseProgram(m_CodeEditor.GetText());
            m_Processor.LoadProgram(m_DLXProgram);
            m_Processor.ExecuteCurrentProgram();

            PHI_LOG_INFO("Executed current program");
        }
        ImGui::SameLine();
        if (ImGui::Button("S"))
        {
            // Step
        }
        ImGui::SameLine();
        if (ImGui::Button("D"))
        {
            // Discard? / Reset
        }

        ImGui::End();
    }
} // namespace dlxemu