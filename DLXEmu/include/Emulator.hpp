#pragma once

#include "Phi/Core/Boolean.hpp"
#include "Window.hpp"

namespace dlxemu
{
    class Emulator
    {
    public:
        ~Emulator();

        phi::Boolean Initialize();

        void Shutdown();

        [[nodiscard]] phi::Boolean IsRunning() const noexcept;

        void MainLoop();

    private:
        Window m_Window;
    };
} // namespace dlxemu
