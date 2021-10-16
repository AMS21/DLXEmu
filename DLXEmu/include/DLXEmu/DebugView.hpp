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
        void RunGuiTest() noexcept;

        Emulator* m_Emulator;

        bool m_TestGuiMode{false};
    };
} // namespace dlxemu
