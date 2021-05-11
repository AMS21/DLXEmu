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

        ~Window() noexcept;

        phi::Boolean Initialize() noexcept;

        void Shutdown() noexcept;

        [[nodiscard]] phi::Boolean IsOpen() const noexcept;

        void Close() noexcept;

        void BeginFrame() noexcept;

        void EndFrame() noexcept;

    private:
        void InitializeImGui() noexcept;

        static void ShutdownImGui() noexcept;

        GLFWwindow*   m_Window{nullptr};
        ImGuiContext* m_ImGuiContext{nullptr};
    };
} // namespace dlxemu
