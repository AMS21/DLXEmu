#include "DLXEmu/Window.hpp"

#include "DLX/Logger.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/boolean.hpp>

#ifdef DLXEMU_USE_GLAD
#    include <glad/gl.h>

static int glad_gl_version = 0;
#endif

#if PHI_PLATFORM_IS(WEB)
#    include <emscripten.h>
#    include <imgui_internal.h>

extern "C" void*     emscripten_GetProcAddress(const char* name_);
extern ImGuiContext* GImGui;
#endif

// GLFW needs to be included after OpenGL
#include <GLFW/glfw3.h>

static phi::boolean glfw_initialized{false};
static phi::boolean imgui_initialized{false};

namespace dlxemu
{
    static void glfw_error_callback(int error, const char* message) noexcept
    {
        (void)error;
        (void)message;

        DLX_ERROR("GLFW error {:d}: {:s}", error, message);
    }

    Window::~Window() noexcept
    {
        Shutdown();
    }

    phi::boolean Window::Initialize() noexcept
    {
        // Set error callback
        glfwSetErrorCallback(&glfw_error_callback);

        if (glfwInit() == GLFW_FALSE)
        {
            DLX_ERROR("Failed to initialize GLFW!");
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
            DLX_ERROR("Failed to create window!");
            return false;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1); // Enable vsync

#ifdef DLXEMU_USE_GLAD
        // Initialize OpenGL using glad
        glad_gl_version = gladLoadGL(glfwGetProcAddress);
        if (glad_gl_version == 0)
        {
            DLX_ERROR("Failed to load OpenGL!");
            return false;
        }

        // Hook unsupported functions
#    if PHI_PLATFORM_IS(WEB)
        glad_glPolygonMode = [](GLenum /*face*/, GLenum /*mode*/) -> void { return; };
#    endif

        DLX_INFO("Successfully loaded OpenGL version {}.{}", GLAD_VERSION_MAJOR(glad_gl_version),
                 GLAD_VERSION_MINOR(glad_gl_version));
#endif

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

    phi::boolean Window::IsOpen() const noexcept
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
        // Rendering
        ImGui::Render();

        int display_w{};
        int display_h{};

        glfwGetFramebufferSize(m_Window, &display_w, &display_h);

#ifdef DLXEMU_USE_GLAD
        constexpr const static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
#endif

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        const ImGuiIO& io = ImGui::GetIO();

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

        if (ImGui::CreateContext() == nullptr)
        {
            DLX_ERROR("Failed to create ImGuiContext");
            return;
        }

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
#if PHI_PLATFORM_IS_NOT(WEB)
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Viewports
#endif

#if PHI_PLATFORM_IS(WEB)
        io.IniFilename = nullptr;

        // Load default config
        constexpr const static char web_default_config[] =
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
        io.IniFilename = "DLXEmu.ini";
#endif

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        PHI_CLANG_SUPPRESS_WARNING_PUSH()
        PHI_CLANG_SUPPRESS_WARNING("-Wunknown-warning-option")
        PHI_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        PHI_CLANG_SUPPRESS_WARNING_POP()

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

        DLX_INFO("Successfully initialized ImGui with glsl {:s}", glsl_version);

        imgui_initialized = true;
    }

    void Window::ShutdownImGui() noexcept
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();
    }
} // namespace dlxemu
