#pragma once

#include "Phi/Core/Boolean.hpp"
#include "Window.hpp"

namespace dlxemu
{
    class Emulator
    {
    public:
        phi::Boolean Initialize();

        void Shutdown();

        [[nodiscard]] phi::Boolean IsRunning() const noexcept;

        void MainLoop();

    private:
        static void glfw_error_callback(int error, const char* description);

    private:
        Window m_Window;
    };
} // namespace dlxemu
