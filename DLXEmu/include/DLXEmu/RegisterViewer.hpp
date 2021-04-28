#pragma once

namespace dlxemu
{
    class Emulator;

    class RegisterViewer
    {
    public:
        explicit RegisterViewer(Emulator* emulator);

        void Render();

    private:
        Emulator* m_Emulator;
    };
} // namespace dlxemu
