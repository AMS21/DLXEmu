#pragma once

namespace dlxemu
{
    class Emulator;

    class RegisterViewer
    {
    public:
        explicit RegisterViewer(Emulator* emulator) noexcept;

        void Render() noexcept;

    private:
        Emulator* m_Emulator;
    };
} // namespace dlxemu
