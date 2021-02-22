#include "Emulator.hpp"

int main(int argc, char* argv[])
{
    dlxemu::Emulator emulator;

    if (!emulator.Initialize())
    {
        return 1;
    }

    while (emulator.IsRunning())
    {
        emulator.MainLoop();
    }

    emulator.Shutdown();

    return 0;
}
