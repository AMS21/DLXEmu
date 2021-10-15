#include "DLXEmu/DebugView.hpp"

#include "DLX/Processor.hpp"
#include "DLXEmu/Emulator.hpp"
#include "imgui.h"

namespace dlxemu
{
    DebugView::DebugView(Emulator* emulator) noexcept
        : m_Emulator{emulator}
    {}

    void DebugView::Render() noexcept
    {
        if (ImGui::Begin("Debug View", &m_Emulator->m_ShowDebugView))
        {
            const dlx::Processor& processor = m_Emulator->GetProcessor();
            const std::string     full_dump =
                    processor.GetProcessorDump() + '\n' + processor.GetRegisterDump() + '\n' +
                    processor.GetMemoryDump() + '\n' + m_Emulator->m_DLXProgram.GetDump();

            ImGui::TextUnformatted(full_dump.c_str());
        }

        ImGui::End();
    }
} // namespace dlxemu
