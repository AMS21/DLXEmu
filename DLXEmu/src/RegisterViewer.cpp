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

            // Int registers
            for (std::uint32_t index{1}; index < 32; ++index)
            {
                ImGui::InputInt(fmt::format("R{}", index).c_str(),
                                reinterpret_cast<std::int32_t*>(
                                        &proc.GetIntRegister(static_cast<dlx::IntRegisterID>(
                                                index + static_cast<std::int32_t>(
                                                                dlx::IntRegisterID::R0)))));
            }
        }

        ImGui::End();
    }
} // namespace dlxemu
