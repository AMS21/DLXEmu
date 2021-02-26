#pragma once

#include <Phi/Core/Boolean.hpp>

struct GLFWwindow;
struct ImGuiContext;

namespace dlxemu
{
    class Window
    {
    public:
        Window() = default;

        ~Window();

        phi::Boolean Initialize();

        void Shutdown();

        [[nodiscard]] phi::Boolean IsOpen() const;

        void BeginFrame();

        void EndFrame();

    private:
        void InitializeImGui();

        static void ShutdownImGui();

        GLFWwindow*   m_Window{nullptr};
        ImGuiContext* m_ImGuiContext{nullptr};
    };
} // namespace dlxemu
