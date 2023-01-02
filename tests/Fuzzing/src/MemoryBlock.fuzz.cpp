#include <DLX/MemoryBlock.hpp>
#include <fmt/core.h>
#include <phi/core/size_t.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <phi/preprocessor/glue.hpp>
#include <phi/type_traits/make_unsigned.hpp>
#include <phi/type_traits/to_unsafe.hpp>

#define GET_T(type, name)                                                                          \
    auto PHI_GLUE(name, _opt) = consume_t<type>(data, size, index);                                \
    if (!PHI_GLUE(name, _opt))                                                                     \
    {                                                                                              \
        return 0;                                                                                  \
    }                                                                                              \
    const type name = PHI_GLUE(name, _opt).value()

#if defined(FUZZ_VERBOSE_LOG)
#    define FUZZ_LOG(...)                                                                          \
        fmt::print(stderr, __VA_ARGS__);                                                           \
        std::putc('\n', stderr);                                                                   \
        std::fflush(stderr)
#else
#    define FUZZ_LOG(...) PHI_EMPTY_MACRO()
#endif

[[nodiscard]] constexpr bool has_x_more(const phi::size_t index, const phi::size_t x,
                                        const phi::size_t size) noexcept
{
    return index + x < size;
}

static const constexpr phi::usize max_size{128u};

template <typename T>
[[nodiscard]] constexpr phi::size_t aligned_size() noexcept
{
    return sizeof(T) + (sizeof(void*) - sizeof(T));
}

template <typename T>
[[nodiscard]] phi::optional<T> consume_t(const phi::uint8_t* data, const phi::size_t size,
                                         phi::size_t& index) noexcept
{
    if (!has_x_more(index, sizeof(T), size))
    {
        return {};
    }

    PHI_ASSUME(index % sizeof(void*) == 0);

    const phi::size_t old_index = index;
    index += aligned_size<T>();

    if constexpr (phi::is_bool_v<T>)
    {
        phi::int8_t value = *reinterpret_cast<const phi::int8_t*>(data + old_index);
        return static_cast<bool>(value);
    }
    else
    {
        return *reinterpret_cast<const T*>(data + old_index);
    }
}

template <typename T>
[[nodiscard]] std::string print_int(const T val) noexcept
{
    return fmt::format(
            "{0:d} 0x{1:02X}", phi::to_unsafe(val),
            static_cast<phi::make_unsigned_t<phi::make_unsafe_t<T>>>(phi::to_unsafe(val)));
}

// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int LLVMFuzzerTestOneInput(const phi::uint8_t* data, phi::size_t size)
{
    dlx::MemoryBlock memory_block{1000u, max_size};

    FUZZ_LOG("Beginning execution");

    for (std::size_t index{0u}; index < size;)
    {
        GET_T(phi::uint32_t, function_index);

        switch (function_index)
        {
            // LoadByte
            case 0: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadByte({:s})", print_int(address));
                (void)memory_block.LoadByte(address);
                break;
            }

            // LoadUnsignedByte
            case 1: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadUnsignedByte({:s})", print_int(address));
                (void)memory_block.LoadUnsignedByte(address);
                break;
            }

            // LoadHalfWord
            case 2: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadHalfWord({:s})", print_int(address));
                (void)memory_block.LoadHalfWord(address);
                break;
            }

                // LoadUnsignedHalfWord
            case 3: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadUnsignedHalfWord({:s})", print_int(address));
                (void)memory_block.LoadUnsignedHalfWord(address);
                break;
            }

            // LoadWord
            case 4: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadWord({:s})", print_int(address));
                (void)memory_block.LoadWord(address);
                break;
            }

            // LoadUnsignedWord
            case 5: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadUnsignedWord({:s})", print_int(address));
                (void)memory_block.LoadUnsignedWord(address);
                break;
            }

            // LoadFloat
            case 6: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadFloat({:s})", print_int(address));
                (void)memory_block.LoadFloat(address);
                break;
            }

            // LoadDouble
            case 7: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("LoadDouble({:s})", print_int(address));
                (void)memory_block.LoadDouble(address);
                break;
            }

            // StoreByte
            case 8: {
                GET_T(phi::size_t, address);
                GET_T(phi::int8_t, value);

                FUZZ_LOG("StoreByte({:s}, {:s})", print_int(address), print_int(value));
                (void)memory_block.StoreByte(address, value);
                break;
            }

            // StoreUnsignedByte
            case 9: {
                GET_T(phi::size_t, address);
                GET_T(phi::uint8_t, value);

                FUZZ_LOG("StoreUnsignedByte({:s}, {:s})", print_int(address), print_int(value));
                (void)memory_block.StoreUnsignedByte(address, value);
                break;
            }

            // StoreHalfWord
            case 10: {
                GET_T(phi::size_t, address);
                GET_T(phi::int16_t, value);

                FUZZ_LOG("StoreHalfWord({:s}, {:s})", print_int(address), print_int(value));
                (void)memory_block.StoreHalfWord(address, value);
                break;
            }

            // StoreUnsignedHalfWord
            case 11: {
                GET_T(phi::size_t, address);
                GET_T(phi::uint16_t, value);

                FUZZ_LOG("StoreUnsignedHalfWord({:s}, {:s})", print_int(address), print_int(value));
                (void)memory_block.StoreUnsignedHalfWord(address, value);
                break;
            }

            // StoreWord
            case 12: {
                GET_T(phi::size_t, address);
                GET_T(phi::int32_t, value);

                FUZZ_LOG("StoreWord({:s}, {:s})", print_int(address), print_int(value));
                (void)memory_block.StoreWord(address, value);
                break;
            }

            // StoreUnsignedWord
            case 13: {
                GET_T(phi::size_t, address);
                GET_T(phi::uint32_t, value);

                FUZZ_LOG("StoreUnsignedWord({:s}, {:s})", print_int(address), print_int(value));
                (void)memory_block.StoreUnsignedWord(address, value);
                break;
            }

            // StoreFloat
            case 14: {
                GET_T(phi::size_t, address);
                GET_T(phi::float32, value);

                FUZZ_LOG("StoreFloat({:s}, {:f})", print_int(address), value);
                (void)memory_block.StoreFloat(address, value);
                break;
            }

            // StoreDouble
            case 15: {
                GET_T(phi::size_t, address);
                GET_T(phi::float64, value);

                FUZZ_LOG("StoreDouble({:s}, {:f})", print_int(address), value);
                (void)memory_block.StoreDouble(address, value);
                break;
            }

            // IsAddressValid
            case 16: {
                GET_T(phi::size_t, address);
                GET_T(phi::size_t, arg_size);

                FUZZ_LOG("IsAddressValid({:s}, {:s})", print_int(address), print_int(arg_size));
                (void)memory_block.IsAddressValid(address, arg_size);
                break;
            }

            // IsAddressAlignedCorrectly
            case 17: {
                GET_T(phi::size_t, address);
                GET_T(phi::size_t, arg_size);

                FUZZ_LOG("IsAddressAlignedCorrectly({:s}, {:s})", print_int(address),
                         print_int(arg_size));
                (void)dlx::MemoryBlock::IsAddressAlignedCorrectly(address, arg_size);
                break;
            }

            // GetStartingAddress
            case 18: {
                FUZZ_LOG("GetStartingAddress()");
                (void)memory_block.GetStartingAddress();
                break;
            }

            // SetStartingAddress
            case 19: {
                GET_T(phi::size_t, address);

                FUZZ_LOG("SetStartingAddress({:s})", print_int(address));
                memory_block.SetStartingAddress(address);
                break;
            }

            // GetSize
            case 20: {
                FUZZ_LOG("GetSize()");
                (void)memory_block.GetSize();
                break;
            }

            // Resize
            case 21: {
                GET_T(phi::size_t, new_size);

                // Reject too big resizes
                if (new_size > max_size)
                {
                    return 0;
                }

                FUZZ_LOG("Resize({:s})", print_int(new_size));
                memory_block.Resize(new_size);
                break;
            }

            // GetRawMemory
            case 22: {
                FUZZ_LOG("GetRawMemory()");
                (void)memory_block.GetRawMemory();
                break;
            }

            // GetRawMemory const
            case 23: {
                const dlx::MemoryBlock const_memory_block = memory_block;
                FUZZ_LOG("const GetRawMemory()");
                (void)memory_block.GetRawMemory();
                break;
            }
        }
    }

    FUZZ_LOG("Finished execution");

    return 0;
}
