#pragma once

namespace dlxemu
{
    class Emulator;

    class DebugView
    {
    public:
        explicit DebugView(Emulator* emulator) noexcept;

        void Render() noexcept;

    private:
        Emulator* m_Emulator;
    };
} // namespace dlxemu
