#include "DLXEmu/MemoryViewer.hpp"

#include "DLX/MemoryBlock.hpp"
#include "DLX/Processor.hpp"
#include "DLXEmu/Emulator.hpp"
#include <imgui.h>

namespace dlxemu
{
    MemoryViewer::MemoryViewer(Emulator* emulator) noexcept
        : m_Emulator(emulator)
    {}

    void MemoryViewer::Render() noexcept
    {
        if (ImGui::Begin("Memory Viewer", &m_Emulator->m_ShowMemoryViewer))
        {
            dlx::MemoryBlock& mem = m_Emulator->GetProcessor().GetMemory();

            std::vector<dlx::MemoryBlock::MemoryByte>& values = mem.GetRawMemory();

            for (std::size_t index{0}; index < values.size(); index += 4)
            {
                ImGui::InputInt(std::to_string((mem.GetStartingAddress() + index).get()).c_str(),
                                reinterpret_cast<std::int32_t*>(&values[index]));
            }
        }

        ImGui::End();
    }
} // namespace dlxemu
