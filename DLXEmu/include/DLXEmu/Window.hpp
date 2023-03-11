#pragma once

#include <phi/core/boolean.hpp>

struct GLFWwindow;
struct ImGuiContext;

namespace dlxemu
{
    class Window
    {
    public:
        Window() = default;

        ~Window() noexcept;

        phi::boolean Initialize() noexcept;

        void Shutdown() noexcept;

        [[nodiscard]] phi::boolean IsOpen() const noexcept;

        void Close() noexcept;

        void BeginFrame() noexcept;

        void EndFrame() noexcept;

    private:
        void InitializeImGui() noexcept;

        static void ShutdownImGui() noexcept;

        GLFWwindow* m_Window{nullptr};
    };
} // namespace dlxemu
