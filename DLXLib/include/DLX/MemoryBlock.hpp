#pragma once

#include "Phi/Core/Boolean.hpp"
#include <Phi/Core/Types.hpp>
#include <optional>
#include <vector>

namespace dlx
{
    class MemoryBlock
    {
    public:
        struct MemoryByte
        {
            constexpr MemoryByte() noexcept
                : signed_value(0)
            {}

            union
            {
                std::int8_t  signed_value;
                std::uint8_t unsigned_value;
            };
        };

        MemoryBlock(phi::usize start_address, phi::usize starting_size) noexcept;

        // Loading
        [[nodiscard]] std::optional<phi::i8>  LoadByte(phi::usize address) const noexcept;
        [[nodiscard]] std::optional<phi::u8>  LoadUnsignedByte(phi::usize address) const noexcept;
        [[nodiscard]] std::optional<phi::i16> LoadHalfWord(phi::usize address) const noexcept;
        [[nodiscard]] std::optional<phi::u16> LoadUnsignedHalfWord(
                phi::usize address) const noexcept;
        [[nodiscard]] std::optional<phi::i32> LoadWord(phi::usize address) const noexcept;
        [[nodiscard]] std::optional<phi::u32> LoadUnsignedWord(phi::usize address) const noexcept;
        [[nodiscard]] std::optional<phi::f32> LoadFloat(phi::usize address) const noexcept;
        [[nodiscard]] std::optional<phi::f64> LoadDouble(phi::usize address) const noexcept;

        // Storing
        phi::Boolean StoreByte(phi::usize address, phi::i8 value) noexcept;
        phi::Boolean StoreUnsignedByte(phi::usize address, phi::u8 value) noexcept;
        phi::Boolean StoreHalfWord(phi::usize address, phi::i16 value) noexcept;
        phi::Boolean StoreUnsignedHalfWord(phi::usize address, phi::u16 value) noexcept;
        phi::Boolean StoreWord(phi::usize address, phi::i32 value) noexcept;
        phi::Boolean StoreUnsignedWord(phi::usize address, phi::u32 value) noexcept;
        phi::Boolean StoreFloat(phi::usize address, phi::f32 value) noexcept;
        phi::Boolean StoreDouble(phi::usize address, phi::f64 value) noexcept;

        [[nodiscard]] phi::Boolean IsAddressValid(phi::usize address,
                                                  phi::usize size) const noexcept;

        void Clear() noexcept;

        [[nodiscard]] phi::usize GetStartingAddress() const noexcept;

        void SetStartingAddress(phi::usize new_starting_address) noexcept;

        [[nodiscard]] std::vector<MemoryByte>& GetRawMemory() noexcept;

        [[nodiscard]] const std::vector<MemoryByte>& GetRawMemory() const noexcept;

    private:
        std::vector<MemoryByte> m_Values;
        phi::usize              m_StartingAddress;
    };
} // namespace dlx