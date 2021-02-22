#pragma once

#include <Phi/Config/Platform.hpp>
#include <Phi/Core/Boolean.hpp>

struct GLFWwindow;

namespace dlxemu
{
    class Window
    {
    public:
        phi::Boolean Initialize();

        void InitializeImGui();

        void Shutdown();

        static void ShutdownImGui();

        [[nodiscard]] phi::Boolean IsOpen() const;

        void BeginFrame();

        void EndFrame();

    private:
#if PHI_PLATFORM_IS_NOT(WEB)
        GLFWwindow* m_Window;
#endif
    };
} // namespace dlxemu
