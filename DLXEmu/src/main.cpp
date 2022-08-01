#include "DLX/Logger.hpp"
#include "DLXEmu/Emulator.hpp"
#include <phi/compiler_support/platform.hpp>
#include <phi/compiler_support/unused.hpp>
#include <cstddef>

#if PHI_PLATFORM_IS(WEB)

#    include <emscripten.h>

static bool inited{false};

extern "C" void main_loop(void* /*data*/) noexcept
{
    static dlxemu::Emulator emulator;

    if (!inited)
    {
        if (!emulator.Initialize())
        {
            DLX_ERROR("Failed to initialize Emulator!");
            std::exit(1);
        }

        inited = true;
        DLX_INFO("Successfully initialized Emulator");
        return;
    }

    emulator.MainLoop();
}
#endif

int main(int argc, char* argv[])
{
    dlx::InitializeDefaultLogger();

#if PHI_PLATFORM_IS(WEB)
    PHI_UNUSED_PARAMETER(argc);
    PHI_UNUSED_PARAMETER(argv);

    emscripten_set_main_loop_arg(main_loop, nullptr, 0, false);
#else
    dlxemu::Emulator emulator{};

    if (emulator.HandleCommandLineArguments(argc, argv) ==
        dlxemu::Emulator::ShouldContinueInitilization::No)
    {
        return 0;
    }

    if (!emulator.Initialize())
    {
        DLX_ERROR("Failed to initialize Emulator!");
        return 1;
    }

    while (emulator.IsRunning())
    {
        emulator.MainLoop();
    }
#endif

    return 0;
}
