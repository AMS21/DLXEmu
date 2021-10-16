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
            const std::string     full_dump = fmt::format(
                    "Processor:\n{:s}\n\nRegister:\n{:s}\nMemory:\n{:s}\nProgram:\n{:s}",
                    processor.GetProcessorDump(), processor.GetRegisterDump(),
                    processor.GetMemoryDump(), m_Emulator->m_DLXProgram.GetDump());

            ImGui::TextUnformatted(full_dump.c_str());
        }

        ImGui::End();
#endif
    }
} // namespace dlxemu
