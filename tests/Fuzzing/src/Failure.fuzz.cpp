#include <cstddef>

extern "C" int LLVMFuzzerTestOneInput(const phi::uint8_t* data, phi::size_t size)
{
    if (size > 4)
    {
        if (data[0] == 'F')
        {
            if (data[1] == 'U')
            {
                if (data[2] == 'Z')
                {
                    if (data[3] == 'Z')
                    {
                        if (data[4] == '!')
                        {
                            __builtin_trap();
                        }
                    }
                }
            }
        }
    }

    return 0;
}
