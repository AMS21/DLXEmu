#include "DLXEmu/RegisterViewer.hpp"

#include "DLXEmu/Emulator.hpp"
#include <DLX/InstructionInfo.hpp>
#include <DLX/RegisterNames.hpp>
#include <imgui.h>
#include <phi/compiler_support/warning.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_GCC_SUPPRESS_WARNING_POP()

namespace dlxemu
{
    RegisterViewer::RegisterViewer(Emulator* emulator) noexcept
        : m_Emulator(emulator)
    {}

    void RegisterViewer::Render() noexcept
    {
        if (ImGui::Begin("Register Viewer", &m_Emulator->m_ShowRegisterViewer))
        {
            dlx::Processor& proc = m_Emulator->GetProcessor();
            if (ImGui::BeginTabBar("RegisterTabs"))
            {
                // Integer Registers
                if (ImGui::BeginTabItem("Integer"))
                {
                    if (m_Emulator->m_DisableEditing)
                    {
                        ImGui::BeginDisabled();
                    }

                    for (phi::uint32_t index{1}; index < 32; ++index)
                    {
                        ImGui::InputInt(fmt::format("R{}", index).c_str(),
                                        reinterpret_cast<phi::int32_t*>(&proc.GetIntRegister(
                                                static_cast<dlx::IntRegisterID>(
                                                        index + static_cast<phi::int32_t>(
                                                                        dlx::IntRegisterID::R0)))));
                    }

                    if (m_Emulator->m_DisableEditing)
                    {
                        ImGui::EndDisabled();
                    }

                    ImGui::EndTabItem();
                }

                // Float Registers
                if (ImGui::BeginTabItem("Float"))
                {
                    if (m_Emulator->m_DisableEditing)
                    {
                        ImGui::BeginDisabled();
                    }

                    for (phi::uint32_t index{0}; index < 32; ++index)
                    {
                        ImGui::InputFloat(
                                fmt::format("F{}", index).c_str(),
                                reinterpret_cast<float*>(
                                        &proc.GetFloatRegister(static_cast<dlx::FloatRegisterID>(
                                                index + static_cast<phi::int32_t>(
                                                                dlx::FloatRegisterID::F0)))));
                    }

                    ImGui::Checkbox("FPSR", reinterpret_cast<bool*>(&proc.GetFPSR()));

                    if (m_Emulator->m_DisableEditing)
                    {
                        ImGui::EndDisabled();
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }

        ImGui::End();
    }
} // namespace dlxemu
