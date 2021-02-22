#include "Emulator.hpp"

#include <Phi/Config/Platform.hpp>
#include <Phi/Core/Log.hpp>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <imgui.h>

namespace dlxemu
{
    phi::Boolean Emulator::Initialize()
    {
        // Initialize logger
        phi::Log::initialize_default_loggers();

        // Initialize Window
        if (!m_Window.Initialize())
        {
            return false;
        }

        // Initialize glad/OpenGL
        if (!gladLoadGL())
        {
            return false;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
        //io.ConfigFlags |=
        //        ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

#if PHI_PLATFORM_IS(MACOS)
        const char* glsl_version = "#version 150";
#else
        const char* glsl_version = "#version 130";
#endif

        // Setup Platform/Renderer backends
        m_Window.InitializeImGui();
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    void Emulator::Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        Window::ShutdownImGui();
        ImGui::DestroyContext();

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