#include "Window.hpp"

#include <Phi/Core/Boolean.hpp>
#include <Phi/Core/Log.hpp>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>

#if PHI_PLATFORM_IS_NOT(WEB)
#    include <GLFW/glfw3.h>
#    include <backends/imgui_impl_glfw.h>
#endif
namespace dlxemu
{
#if PHI_PLATFORM_IS_NOT(WEB)
    static void glfw_error_callback(int error, const char* message)
    {
        PHI_LOG_ERROR("GLFW error {}: {}", error, message);
    }
#endif

    phi::Boolean Window::Initialize()
    {
        // Don't need to create a windows at all when using Emscripten
#if PHI_PLATFORM_IS_NOT(WEB)
        // Set error callback
        glfwSetErrorCallback(&glfw_error_callback);

        if (!glfwInit())
        {
            return false;
        }

        // Decide GL versions
#    if PHI_PLATFORM_IS(MACOS)
        // GL 3.2
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#    else
        // GL 3.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#    endif

        // Create window with graphics context
        m_Window = glfwCreateWindow(1280, 720, "DLXEmu", nullptr, nullptr);
        if (m_Window == nullptr)
        {
            return false;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1); // Enable vsync}
#endif

        return true;
    }

    void Window::InitializeImGui()
    {
#if PHI_PLATFORM_IS_NOT(WEB)
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
#endif
    }

    void Window::Shutdown()
    {
#if PHI_PLATFORM_IS_NOT(WEB)
        glfwDestroyWindow(m_Window);
        glfwTerminate();
#endif
    }

    void Window::ShutdownImGui()
    {
#if PHI_PLATFORM_IS_NOT(WEB)
        ImGui_ImplGlfw_Shutdown();
#endif
    }

    phi::Boolean Window::IsOpen() const
    {
#if PHI_PLATFORM_IS(WEB)
        return true;
#else
        return glfwWindowShouldClose(m_Window) == 0;
#endif
    }

    void Window::BeginFrame()
    {
#if PHI_PLATFORM_IS_NOT(WEB)
        glfwPollEvents();
#endif

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
#if PHI_PLATFORM_IS_NOT(WEB)
        ImGui_ImplGlfw_NewFrame();
#endif
        ImGui::NewFrame();
    }

    void Window::EndFrame()
    {
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Rendering
        ImGui::Render();
        int display_w{};
        int display_h{};

#if PHI_PLATFORM_IS_NOT(WEB)
        glfwGetFramebufferSize(m_Window, &display_w, &display_h);
#endif
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#if PHI_PLATFORM_IS_NOT(WEB)
        ImGuiIO& io = ImGui::GetIO();

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(m_Window);
#endif
    }
} // namespace dlxemu
