#include "Emulator.hpp"

#include <Phi/Config/Platform.hpp>
#include <Phi/Core/Log.hpp>
#include <imgui.h>

namespace dlxemu
{
    Emulator::~Emulator()
    {
        Shutdown();
    }

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

    void Emulator::Shutdown()
    {
        m_Window.Shutdown();
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

        static bool show{true};
        ImGui::ShowDemoWindow(&show);

        m_Window.EndFrame();
    }
} // namespace dlxemu