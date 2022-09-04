#pragma once

#include <phi/core/boolean.hpp>

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

#if defined(PHI_DEBUG)
        bool m_TestGuiMode{false};
#endif
    };
} // namespace dlxemu
