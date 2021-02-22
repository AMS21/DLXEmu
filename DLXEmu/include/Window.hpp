#pragma once

#include <Phi/Core/Boolean.hpp>

struct GLFWwindow;

namespace dlxemu
{
    class Window
    {
    public:
        phi::Boolean Initialize();

        void Shutdown();

        [[nodiscard]] phi::Boolean IsOpen() const;

        void BeginFrame();

        void EndFrame();

    private:
        void InitializeImGui();

        static void ShutdownImGui();

        GLFWwindow* m_Window;
    };
} // namespace dlxemu
