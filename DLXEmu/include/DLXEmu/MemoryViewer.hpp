#pragma once

namespace dlxemu
{
    class Emulator;

    class MemoryViewer
    {
    public:
        explicit MemoryViewer(Emulator* emulator);

        void Render();

    private:
        Emulator* m_Emulator;
    };
} // namespace dlxemu