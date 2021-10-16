#include "DLXEmu/DebugView.hpp"

#include "DLX/Processor.hpp"
#include "DLXEmu/Emulator.hpp"
#include "imgui.h"
#include <spdlog/fmt/bundled/core.h>

namespace dlxemu
{
    DebugView::DebugView(Emulator* emulator) noexcept
        : m_Emulator{emulator}
    {}

    void DebugView::Render() noexcept
    {
#if defined(PHI_DEBUG)
        if (ImGui::Begin("Debug View", &m_Emulator->m_ShowDebugView))
        {
            const dlx::Processor& processor = m_Emulator->GetProcessor();

            // Dumps
            if (ImGui::CollapsingHeader("Processor Dump"))
            {
                const std::string dump = processor.GetProcessorDump();
                ImGui::TextUnformatted(dump.c_str());
            }

            if (ImGui::CollapsingHeader("Register Dump"))
            {
                const std::string dump = processor.GetRegisterDump();
                ImGui::TextUnformatted(dump.c_str());
            }

            if (ImGui::CollapsingHeader("Memory Dump"))
            {
                const std::string dump = processor.GetMemoryDump();
                ImGui::TextUnformatted(dump.c_str());
            }

            if (ImGui::CollapsingHeader("Program Dump"))
            {
                const std::string dump = m_Emulator->m_DLXProgram.GetDump();
                ImGui::TextUnformatted(dump.c_str());
            }
        }

        ImGui::End();
#endif
    }
} // namespace dlxemu
