#include "SetupImGui.hpp"

#include <catch2/catch_test_macros.hpp>
#include <imgui.h>
#include <imgui_internal.h>

void BeginImGui() noexcept
{
    ImGuiContext* ctx = ImGui::CreateContext();
    REQUIRE(ctx);

    ImGuiIO& io = ImGui::GetIO();

    // Enforce valid display size
    io.DisplaySize.x = 1024.0f;
    io.DisplaySize.y = 768.0f;

    // Enfore valid DeltaTime
    io.DeltaTime = 1.0f / 60.0f;

    // Enforce valid space key mapping
    io.KeyMap[ImGuiKey_Space] = 0;

    // Don't save any config
    io.IniFilename = nullptr;

    // Build atlas
    unsigned char* tex_pixels{nullptr};
    int            tex_w{0};
    int            tex_h{0};
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

    ImGui::NewFrame();
}

void EndImgui() noexcept
{
    ImGui::EndFrame();

    ImGui::DestroyContext(ImGui::GetCurrentContext());
}