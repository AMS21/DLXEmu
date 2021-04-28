#include "DLXEmu/Emulator.hpp"

#include <Phi/Config/Platform.hpp>
#include <Phi/Core/Log.hpp>
#include <imgui.h>

namespace dlxemu
{
    Emulator::Emulator() noexcept
        : m_CodeEditor(this)
        , m_MemoryViewer(this)
    {}

    phi::Boolean Emulator::Initialize()
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

    void Emulator::MainLoop()
    {
        m_Window.BeginFrame();

        // Add docking space
        const auto* viewport = ImGui::GetWindowViewport();
        ImGui::DockSpaceOverViewport(viewport);

        // Render our stuff
        m_CodeEditor.Render();
        m_MemoryViewer.Render();

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
} // namespace dlxemu