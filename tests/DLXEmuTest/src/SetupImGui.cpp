#include "SetupImGui.hpp"

#include <catch2/catch_test_macros.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <phi/compiler_support/unused.hpp>

void BeginImGui() noexcept
{
    IMGUI_CHECKVERSION();
    REQUIRE(GImGui == nullptr);

    ImGuiContext* ctx = ImGui::CreateContext();
    REQUIRE(ctx);

    // Setup Config
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    // Enforce valid display size
    io.DisplaySize.x = 1024.0f;
    io.DisplaySize.y = 768.0f;

    // Enfore valid DeltaTime
    io.DeltaTime = 1.0f / 60.0f;

    // Don't save any config
    io.IniFilename = nullptr;

    // SetStyle
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Build atlas
    unsigned char* tex_pixels{nullptr};
    int            tex_w{0};
    int            tex_h{0};
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

    ImGui::NewFrame();
}

void EndImgui() noexcept
{
    // Render data
    ImGui::Render();

    volatile ImDrawData* draw_data = ImGui::GetDrawData();
    PHI_UNUSED_VARIABLE(draw_data);

    ImGuiIO& io = ImGui::GetIO();
    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    ImGui::EndFrame();

    // Destroy context
    ImGui::DestroyContext(ImGui::GetCurrentContext());
}
