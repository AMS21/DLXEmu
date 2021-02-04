#pragma once

#include <Phi/Core/Types.hpp>
#include <optional>
#include <vector>

namespace dlx
{
    class MemoryBlock
    {
        struct MemoryByte
        {
            MemoryByte()
                : signed_value(0)
            {}

            union
            {
                std::int8_t  signed_value;
                std::uint8_t unsigned_value;
            };
        };

    public:
        MemoryBlock(phi::usize start_address, phi::usize starting_size);

        // Loading
        std::optional<phi::i8>  LoadByte(phi::usize address) const;
        std::optional<phi::u8>  LoadUnsignedByte(phi::usize address) const;
        std::optional<phi::i16> LoadHalfWord(phi::usize address) const;
        std::optional<phi::u16> LoadUnsignedHalfWord(phi::usize address) const;
        std::optional<phi::i32> LoadWord(phi::usize address) const;
        std::optional<phi::u32> LoadUnsignedWord(phi::usize address) const;

        // Storing
        phi::Boolean StoreByte(phi::usize address, phi::i8 value);
        phi::Boolean StoreUnsignedByte(phi::usize address, phi::u8 value);
        phi::Boolean StoreHalfWord(phi::usize address, phi::i16 value);
        phi::Boolean StoreUnsignedHalfWord(phi::usize address, phi::u16 value);
        phi::Boolean StoreWord(phi::usize address, phi::i32 value);
        phi::Boolean StoreUnsignedWord(phi::usize address, phi::u32 value);

        phi::Boolean IsAddressValid(phi::usize address, phi::usize size) const;

        void Clear();

        std::vector<MemoryByte> m_Values;
        phi::usize              m_StartingAddress;
    };
} // namespace dlx