#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
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
                        if (data[4] == 'A')
                        {
                            while (true)
                            {}
                        }
                    }
                }
            }
        }
    }

    return 0;
}
