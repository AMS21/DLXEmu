#include <phi/test/test_macros.hpp>

#include "SetupImGui.hpp"
#include <imgui.h>

TEST_CASE("SetupImGui empty")
{
    BeginImGui();
    EndImGui();
}

TEST_CASE("SetupImGui simple")
{
    BeginImGui();

    ImGui::Begin("Test");
    ImGui::End();

    EndImGui();
}

TEST_CASE("SetupImGui with viewports")
{
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    BeginImGui();
    EndImGui();

    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;

    BeginImGui();
    EndImGui();
}
