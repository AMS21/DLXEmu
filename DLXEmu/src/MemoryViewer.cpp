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
        ImGui::Begin("Memory Viewer");

        dlx::MemoryBlock mem = m_Emulator->GetProcessor().GetMemory();

        auto& values = mem.GetRawMemory();

        for (std::size_t index{0}; index < values.size(); index += 4)
        {
            std::int32_t val = mem.LoadWord(mem.GetStartingAddress() + index)->get();

            ImGui::InputInt(std::to_string((mem.GetStartingAddress() + index).get()).c_str(), &val);
        }

        ImGui::End();
    }
} // namespace dlxemu
