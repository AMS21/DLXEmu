#include "DLXEmu/RegisterViewer.hpp"

#include "DLXEmu/Emulator.hpp"
#include <DLX/InstructionInfo.hpp>
#include <DLX/RegisterNames.hpp>
#include <imgui.h>
#include <spdlog/fmt/fmt.h>

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
                    for (std::uint32_t index{1}; index < 32; ++index)
                    {
                        ImGui::InputInt(fmt::format("R{}", index).c_str(),
                                        reinterpret_cast<std::int32_t*>(&proc.GetIntRegister(
                                                static_cast<dlx::IntRegisterID>(
                                                        index + static_cast<std::int32_t>(
                                                                        dlx::IntRegisterID::R0)))));
                    }

                    ImGui::EndTabItem();
                }

                // Float Registers
                if (ImGui::BeginTabItem("Float"))
                {
                    for (std::uint32_t index{0}; index < 32; ++index)
                    {
                        ImGui::InputFloat(
                                fmt::format("F{}", index).c_str(),
                                reinterpret_cast<float*>(
                                        &proc.GetFloatRegister(static_cast<dlx::FloatRegisterID>(
                                                index + static_cast<std::int32_t>(
                                                                dlx::FloatRegisterID::F0)))));
                    }

                    ImGui::Checkbox("FPSR", reinterpret_cast<bool*>(&proc.GetFPSR()));

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }

        ImGui::End();
    }
} // namespace dlxemu
