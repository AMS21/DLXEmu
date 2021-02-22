#include "Emulator.hpp"
#include "Phi/Core/Log.hpp"
#include <Phi/Config/Platform.hpp>
#include <cstddef>

#if PHI_PLATFORM_IS(WEB)
#    include "emscripten.h"
#endif

#if PHI_PLATFORM_IS(WEB)
extern "C" void main_loop(void* data)
{
    dlxemu::Emulator* emulator = reinterpret_cast<dlxemu::Emulator*>(data);

    emulator->MainLoop();
}
#endif

int main(int argc, char* argv[])
{
    // Initialize logger
    phi::Log::initialize_default_loggers();

    dlxemu::Emulator emulator{};
    if (!emulator.Initialize())
    {
        PHI_LOG_ERROR("Failed to Emulator!");
        return 1;
    }

#if PHI_PLATFORM_IS(WEB)
    emscripten_set_main_loop_arg(main_loop, &emulator, 0, false);
#else

    while (emulator.IsRunning())
    {
        emulator.MainLoop();
    }
#endif

    return 0;
}
