#pragma once

namespace dlxemu
{
    class Emulator;

    class MemoryViewer
    {
    public:
        explicit MemoryViewer(Emulator* emulator) noexcept;

        void Render() noexcept;

    private:
        Emulator* m_Emulator;
    };
} // namespace dlxemu
