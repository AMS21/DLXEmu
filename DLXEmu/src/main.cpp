#include "DLXEmu/Emulator.hpp"
#include <Phi/Config/Platform.hpp>
#include <Phi/Core/Log.hpp>
#include <cstddef>

#if PHI_PLATFORM_IS(WEB)
#    include <emscripten.h>
#endif

#if PHI_PLATFORM_IS(WEB)

static bool inited{false};

extern "C" void main_loop(void* data) noexcept
{
    dlxemu::Emulator* emulator = reinterpret_cast<dlxemu::Emulator*>(data);

    if (!inited)
    {
        if (!emulator->Initialize())
        {
            PHI_LOG_ERROR("Failed to initialize Emulator!");
            std::exit(1);
        }

        inited = true;
        PHI_LOG_INFO("Successfully initialized Emulator");
        return;
    }

    emulator->MainLoop();
}
#endif

int main(int argc, char* argv[])
{
    // Initialize logger
    phi::Log::initialize_default_loggers();

    dlxemu::Emulator emulator{};

#if PHI_PLATFORM_IS(WEB)
    emscripten_set_main_loop_arg(main_loop, &emulator, 0, false);
#else
    if (!emulator.Initialize())
    {
        PHI_LOG_ERROR("Failed to initialize Emulator!");
        return 1;
    }

    while (emulator.IsRunning())
    {
        emulator.MainLoop();
    }
#endif

    return 0;
}
