#include "Window.hpp"

#include <Phi/Core/Boolean.hpp>
#include <Phi/Core/Log.hpp>
#include <backends/imgui_impl_glfw.h>

#if PHI_PLATFORM_IS(WEB)
#    include <backends/imgui_impl_wgpu.h>
#    include <emscripten.h>
#    include <emscripten/html5.h>
#    include <emscripten/html5_webgpu.h>
#    include <webgpu/webgpu.h>
#    include <webgpu/webgpu_cpp.h>
#else
#    include <backends/imgui_impl_opengl3.h>
#    include <glad/glad.h>
#endif

// GLFW needs to be included after opengl
#include <GLFW/glfw3.h>

namespace dlxemu
{
    static void glfw_error_callback(int error, const char* message)
    {
        PHI_LOG_ERROR("GLFW error {}: {}", error, message);
    }

#if PHI_PLATFORM_IS(WEB)
    static void print_wgpu_error(WGPUErrorType error_type, const char* message, void*)
    {
        const char* error_type_lbl = "";
        switch (error_type)
        {
            case WGPUErrorType_Validation:
                error_type_lbl = "Validation";
                break;
            case WGPUErrorType_OutOfMemory:
                error_type_lbl = "Out of memory";
                break;
            case WGPUErrorType_Unknown:
                error_type_lbl = "Unknown";
                break;
            case WGPUErrorType_DeviceLost:
                error_type_lbl = "Device lost";
                break;
            default:
                error_type_lbl = "Unknown";
        }

        PHI_LOG_ERROR("{} error: {}", error_type_lbl, message);
    }

    // Global WebGPU required states
    static WGPUDevice    wgpu_device            = NULL;
    static WGPUSurface   wgpu_surface           = NULL;
    static WGPUSwapChain wgpu_swap_chain        = NULL;
    static int           wgpu_swap_chain_width  = 0;
    static int           wgpu_swap_chain_height = 0;

    static phi::Boolean init_wgpu()
    {
        wgpu_device = emscripten_webgpu_get_device();
        if (!wgpu_device)
            return false;

        wgpuDeviceSetUncapturedErrorCallback(wgpu_device, print_wgpu_error, NULL);

        // Use C++ wrapper due to misbehavior in Emscripten.
        // Some offset computation for wgpuInstanceCreateSurface in JavaScript
        // seem to be inline with struct alignments in the C++ structure
        wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc = {};
        html_surface_desc.selector                                      = "#canvas";

        wgpu::SurfaceDescriptor surface_desc = {};
        surface_desc.nextInChain             = &html_surface_desc;

        // Use 'null' instance
        wgpu::Instance instance = {};
        wgpu_surface            = instance.CreateSurface(&surface_desc).Release();

        return true;
    }
#endif

    phi::Boolean Window::Initialize()
    {
        // Set error callback
        glfwSetErrorCallback(&glfw_error_callback);

        if (!glfwInit())
        {
            PHI_LOG_ERROR("Failed to initialize GLFW!");
            return false;
        }

        // Decide GL versions
#if PHI_PLATFORM_IS(MACOS)
        // GL 3.2
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
        // GL 3.0
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

#if PHI_PLATFORM_IS(WEB)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

        // Create window with graphics context
        m_Window = glfwCreateWindow(1280, 720, "DLXEmu", nullptr, nullptr);
        if (m_Window == nullptr)
        {
            PHI_LOG_ERROR("Failed to create window!");
            return false;
        }

#if PHI_PLATFORM_IS(WEB)
        if (!init_wgpu())
        {
            PHI_LOG_ERROR("Failed to load WGPU!");
            return false;
        }

        glfwShowWindow(m_Window);
#else
        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(1); // Enable vsync}

        // Initilaize OpenGL using glad
        if (!gladLoadGL())
        {
            PHI_LOG_ERROR("Failed to load OpenGL!");
            return false;
        }

        PHI_LOG_INFO("Successfully loaded OpenGL version {}.{}", GLVersion.major, GLVersion.minor);
#endif

        InitializeImGui();

        return true;
    }

    void Window::Shutdown()
    {
        ShutdownImGui();
        ImGui::DestroyContext();

        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
        }

        glfwTerminate();
    }

    phi::Boolean Window::IsOpen() const
    {
        return glfwWindowShouldClose(m_Window) == 0;
    }

    void Window::BeginFrame()
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
#if PHI_PLATFORM_IS(WEB)
        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);

        // React to changes in screen size
        if (width != wgpu_swap_chain_width && height != wgpu_swap_chain_height)
        {
            ImGui_ImplWGPU_InvalidateDeviceObjects();

            if (wgpu_swap_chain)
            {
                wgpuSwapChainRelease(wgpu_swap_chain);
            }

            wgpu_swap_chain_width  = width;
            wgpu_swap_chain_height = height;

            WGPUSwapChainDescriptor swap_chain_desc = {};
            swap_chain_desc.usage                   = WGPUTextureUsage_OutputAttachment;
            swap_chain_desc.format                  = WGPUTextureFormat_RGBA8Unorm;
            swap_chain_desc.width                   = width;
            swap_chain_desc.height                  = height;
            swap_chain_desc.presentMode             = WGPUPresentMode_Fifo;
            wgpu_swap_chain =
                    wgpuDeviceCreateSwapChain(wgpu_device, wgpu_surface, &swap_chain_desc);

            ImGui_ImplWGPU_CreateDeviceObjects();
        }

        ImGui_ImplWGPU_NewFrame();
#else
        ImGui_ImplOpenGL3_NewFrame();
#endif

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void Window::EndFrame()
    {
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Rendering
        ImGui::Render();

#if PHI_PLATFORM_IS(WEB)
        WGPURenderPassColorAttachmentDescriptor color_attachments = {};
        color_attachments.loadOp                                  = WGPULoadOp_Clear;
        color_attachments.storeOp                                 = WGPUStoreOp_Store;
        color_attachments.clearColor                              = {clear_color.x * clear_color.w,
                                        clear_color.y * clear_color.w,
                                        clear_color.z * clear_color.w, clear_color.w};
        color_attachments.attachment = wgpuSwapChainGetCurrentTextureView(wgpu_swap_chain);
        WGPURenderPassDescriptor render_pass_desc = {};
        render_pass_desc.colorAttachmentCount     = 1;
        render_pass_desc.colorAttachments         = &color_attachments;
        render_pass_desc.depthStencilAttachment   = NULL;

        WGPUCommandEncoderDescriptor enc_desc = {};
        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpu_device, &enc_desc);

        WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
        wgpuRenderPassEncoderEndPass(pass);

        WGPUCommandBufferDescriptor cmd_buffer_desc = {};
        WGPUCommandBuffer cmd_buffer = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
        WGPUQueue         queue      = wgpuDeviceGetDefaultQueue(wgpu_device);
        wgpuQueueSubmit(queue, 1, &cmd_buffer);
#else
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
#endif
    }

    void Window::InitializeImGui()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
        //io.ConfigFlags |=
        //        ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

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
#if PHI_PLATFORM_IS(WEB)
        ImGui_ImplGlfw_InitForOther(m_Window, true);
        ImGui_ImplWGPU_Init(wgpu_device, 3, WGPUTextureFormat_RGBA8Unorm);
#else
#    if PHI_PLATFORM_IS(MACOS)
        const char* glsl_version = "#version 150";
#    else
        const char* glsl_version = "#version 130";
#    endif

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
#endif
    }

    void Window::ShutdownImGui()
    {
        ImGui_ImplGlfw_Shutdown();

#if PHI_PLATFORM_IS_NOT(WEB)
        ImGui_ImplOpenGL3_Shutdown();
#endif
    }
} // namespace dlxemu
