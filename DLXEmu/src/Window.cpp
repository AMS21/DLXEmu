#include "DLXEmu/Window.hpp"

#include <Phi/Core/Boolean.hpp>
#include <Phi/Core/Log.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <imgui.h>

#if PHI_PLATFORM_IS(WEB)
#    include <emscripten.h>
#    include <imgui_internal.h>

extern "C" void*     emscripten_GetProcAddress(const char* name_);
extern ImGuiContext* GImGui;
#endif

// GLFW needs to be included after opengl
#include <GLFW/glfw3.h>

static bool glfw_initialized{false};
static bool imgui_initialized{false};

namespace dlxemu
{
    static void glfw_error_callback(int error, const char* message) noexcept
    {
        PHI_LOG_ERROR("GLFW error {}: {}", error, message);
    }

    Window::~Window() noexcept
    {
        Shutdown();
    }

    phi::Boolean Window::Initialize() noexcept
    {
        // Set error callback
        glfwSetErrorCallback(&glfw_error_callback);

        if (!glfwInit())
        {
            PHI_LOG_ERROR("Failed to initialize GLFW!");
            return false;
        }

        glfw_initialized = true;

        // Decide GL versions
#if PHI_PLATFORM_IS(MACOS)
        // GL 3.2
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#elif PHI_PLATFORM_IS(WEB)
        // GL ES 2.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
        // GL 3.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

        // Create window with graphics context
        m_Window = glfwCreateWindow(1280, 720, "DLXEmu", nullptr, nullptr);
        if (m_Window == nullptr)
        {
            PHI_LOG_ERROR("Failed to create window!");
            return false;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1); // Enable vsync

        // Initilaize OpenGL using glad
#if PHI_PLATFORM_IS(WEB)
        if (!gladLoadGLLoader((GLADloadproc)emscripten_GetProcAddress))
#else
        if (!gladLoadGL())
#endif
        {
            PHI_LOG_ERROR("Failed to load OpenGL!");
            return false;
        }

        // Hook unsupported functions functions
#if PHI_PLATFORM_IS(WEB)
        glad_glPolygonMode = [](GLenum /*face*/, GLenum /*mode*/) -> void { return; };
#endif

        PHI_LOG_INFO("Successfully loaded OpenGL version {}.{}", GLVersion.major, GLVersion.minor);

        InitializeImGui();

        return true;
    }

    void Window::Shutdown() noexcept
    {
        if (imgui_initialized)
        {
            ShutdownImGui();
        }

        if (m_Window != nullptr)
        {
            glfwDestroyWindow(m_Window);
        }

        if (glfw_initialized)
        {
            glfwTerminate();
        }
    }

    phi::Boolean Window::IsOpen() const noexcept
    {
        return glfwWindowShouldClose(m_Window) == 0;
    }

    void Window::Close() noexcept
    {
        glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
    }

    void Window::BeginFrame() noexcept
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

#if PHI_PLATFORM_IS(WEB)
        if (GImGui->IO.DeltaTime <= 0.0f)
        {
            GImGui->IO.DeltaTime = 0.001f;
        }
#endif

        ImGui::NewFrame();
    }

    void Window::EndFrame() noexcept
    {
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Rendering
        ImGui::Render();

        int display_w{};
        int display_h{};

        glfwGetFramebufferSize(m_Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    }

    void Window::InitializeImGui() noexcept
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        m_ImGuiContext = ImGui::CreateContext();
        if (m_ImGuiContext == nullptr)
        {
            PHI_LOG_ERROR("Failed to create ImGuiContext");
            return;
        }

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking

#if PHI_PLATFORM_IS(WEB)
        io.IniFilename = nullptr;

        // Load default config
        constexpr char web_default_config[] =
                R"([Window][DockSpaceViewport_11111111]
Pos=0,19
Size=1280,701
Collapsed=0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Window][Code Editor]
Pos=0,74
Size=962,646
Collapsed=0
DockId=0x00000002,0

[Window][Register Viewer]
Pos=964,19
Size=149,701
Collapsed=0
DockId=0x00000004,0

[Window][Memory Viewer]
Pos=1115,19
Size=165,701
Collapsed=0
DockId=0x00000006,0

[Window][Control Panel]
Pos=0,19
Size=962,53
Collapsed=0
DockId=0x00000001,0

[Docking][Data]
DockSpace       ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,19 Size=1280,701 Split=X
  DockNode      ID=0x00000005 Parent=0x8B93E3BD SizeRef=1113,701 Split=X
    DockNode    ID=0x00000003 Parent=0x00000005 SizeRef=962,701 Split=Y
      DockNode  ID=0x00000001 Parent=0x00000003 SizeRef=1280,53 Selected=0x919FA1B7
      DockNode  ID=0x00000002 Parent=0x00000003 SizeRef=1280,646 CentralNode=1 Selected=0x2867833B
    DockNode    ID=0x00000004 Parent=0x00000005 SizeRef=149,701 Selected=0x25A54FD5
  DockNode      ID=0x00000006 Parent=0x8B93E3BD SizeRef=165,701 Selected=0xE94D7839
)";

        ImGui::LoadIniSettingsFromMemory(web_default_config);
#else
        io.IniFilename           = "DLXEmu.ini";
#endif

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

        // Setup Platform/Renderer backends
#if PHI_PLATFORM_IS(MACOS)
        const char* glsl_version = "#version 150";
#elif PHI_PLATFORM_IS(WEB)
        const char* glsl_version = "#version 100";
#else
        const char* glsl_version = "#version 130";
#endif

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        PHI_LOG_INFO("Successfully initialized ImGui with glsl {}", glsl_version);

        imgui_initialized = true;
    }

    void Window::ShutdownImGui() noexcept
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();

        ImGui::DestroyContext();
    }
} // namespace dlxemu
