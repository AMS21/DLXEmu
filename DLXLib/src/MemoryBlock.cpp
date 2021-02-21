#include "DLX/MemoryBlock.hpp"
#include "Phi/Core/Boolean.hpp"
#include "Phi/Core/Log.hpp"
#include <cstdint>

namespace dlx
{
    MemoryBlock::MemoryBlock(phi::usize start_address, phi::usize starting_size)
        : m_StartingAddress(start_address)
    {
        m_Values.resize(starting_size.get());
    }

    std::optional<phi::i8> MemoryBlock::LoadByte(phi::usize address) const
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        return m_Values.at((address - m_StartingAddress).get()).signed_value;
    }

    std::optional<phi::u8> MemoryBlock::LoadUnsignedByte(phi::usize address) const
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        return m_Values.at((address - m_StartingAddress).get()).unsigned_value;
    }

    std::optional<phi::i16> MemoryBlock::LoadHalfWord(phi::usize address) const
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        phi::usize   start_address = address - m_StartingAddress;
        std::int8_t  first_byte    = m_Values.at(start_address.get()).signed_value;
        std::int8_t  second_byte   = m_Values.at((start_address + 1u).get()).signed_value;
        std::int16_t result        = first_byte << 8 | second_byte;

        return result;
    }

    std::optional<phi::u16> MemoryBlock::LoadUnsignedHalfWord(phi::usize address) const
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        phi::usize    start_address = address - m_StartingAddress;
        std::uint8_t  first_byte    = m_Values.at(start_address.get()).unsigned_value;
        std::uint8_t  second_byte   = m_Values.at((start_address + 1u).get()).unsigned_value;
        std::uint16_t result        = first_byte << 8 | second_byte;

        return result;
    }

    std::optional<phi::i32> MemoryBlock::LoadWord(phi::usize address) const
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        phi::usize   start_address = address - m_StartingAddress;
        std::int8_t  first_byte    = m_Values.at(start_address.get()).signed_value;
        std::int8_t  second_byte   = m_Values.at((start_address + 1u).get()).signed_value;
        std::int8_t  third_byte    = m_Values.at((start_address + 2u).get()).signed_value;
        std::int8_t  fourth_byte   = m_Values.at((start_address + 3u).get()).signed_value;
        std::int32_t result = first_byte << 24 | second_byte << 16 | third_byte << 8 | fourth_byte;

        return result;
    }

    std::optional<phi::u32> MemoryBlock::LoadUnsignedWord(phi::usize address) const
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        phi::usize    start_address = address - m_StartingAddress;
        std::uint8_t  first_byte    = m_Values.at(start_address.get()).unsigned_value;
        std::uint8_t  second_byte   = m_Values.at((start_address + 1u).get()).unsigned_value;
        std::uint8_t  third_byte    = m_Values.at((start_address + 2u).get()).unsigned_value;
        std::uint8_t  fourth_byte   = m_Values.at((start_address + 3u).get()).unsigned_value;
        std::uint32_t result = first_byte << 24 | second_byte << 16 | third_byte << 8 | fourth_byte;

        return result;
    }

    std::optional<phi::f32> MemoryBlock::LoadFloat(phi::usize address) const
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        phi::usize    start_address = address - m_StartingAddress;
        std::uint8_t  first_byte    = m_Values.at(start_address.get()).unsigned_value;
        std::uint8_t  second_byte   = m_Values.at((start_address + 1u).get()).unsigned_value;
        std::uint8_t  third_byte    = m_Values.at((start_address + 2u).get()).unsigned_value;
        std::uint8_t  fourth_byte   = m_Values.at((start_address + 3u).get()).unsigned_value;
        std::uint32_t result = first_byte << 24 | second_byte << 16 | third_byte << 8 | fourth_byte;

        return *reinterpret_cast<float*>(&result);
    }

    std::optional<phi::f64> MemoryBlock::LoadDouble(phi::usize address) const
    {
        if (!IsAddressValid(address, 8u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        phi::f32 first_value  = LoadFloat(address).value();
        phi::f32 second_value = LoadFloat(address + 4u).value();

        std::uint32_t first_bits  = *reinterpret_cast<std::uint32_t*>(&first_value);
        std::uint32_t second_bits = *reinterpret_cast<std::uint32_t*>(&second_value);

        std::uint64_t final_bits = (static_cast<std::uint64_t>(second_bits) << 32u) | first_bits;

        return *reinterpret_cast<double*>(&final_bits);
    }

    phi::Boolean MemoryBlock::StoreByte(phi::usize address, phi::i8 value)
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        m_Values.at((address - m_StartingAddress).get()).signed_value = value.get();
        return true;
    }

    phi::Boolean MemoryBlock::StoreUnsignedByte(phi::usize address, phi::u8 value)
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        m_Values.at((address - m_StartingAddress).get()).unsigned_value = value.get();
        return true;
    }

    phi::Boolean MemoryBlock::StoreHalfWord(phi::usize address, phi::i16 value)
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        phi::usize  start_address = address - m_StartingAddress;
        std::int8_t first_byte    = (value.get() & 0b11111111'00000000) >> 8;
        std::int8_t second_byte   = value.get() & 0b00000000'11111111;

        m_Values.at(start_address.get()).signed_value        = first_byte;
        m_Values.at((start_address + 1u).get()).signed_value = second_byte;

        return true;
    }

    phi::Boolean MemoryBlock::StoreUnsignedHalfWord(phi::usize address, phi::u16 value)
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        phi::usize   start_address = address - m_StartingAddress;
        std::uint8_t first_byte    = (value.get() & 0b11111111'00000000) >> 8;
        std::uint8_t second_byte   = value.get() & 0b00000000'11111111;

        m_Values.at(start_address.get()).unsigned_value        = first_byte;
        m_Values.at((start_address + 1u).get()).unsigned_value = second_byte;

        return true;
    }

    phi::Boolean MemoryBlock::StoreWord(phi::usize address, phi::i32 value)
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        phi::usize  start_address = address - m_StartingAddress;
        std::int8_t first_byte    = (value.get() & 0b11111111'00000000'00000000'00000000) >> 24;
        std::int8_t second_byte   = (value.get() & 0b00000000'11111111'00000000'00000000) >> 16;
        std::int8_t third_byte    = (value.get() & 0b00000000'00000000'11111111'00000000) >> 8;
        std::int8_t fourth_byte   = value.get() & 0b00000000'00000000'00000000'11111111;

        m_Values.at(start_address.get()).signed_value        = first_byte;
        m_Values.at((start_address + 1u).get()).signed_value = second_byte;
        m_Values.at((start_address + 2u).get()).signed_value = third_byte;
        m_Values.at((start_address + 3u).get()).signed_value = fourth_byte;

        return true;
    }

    phi::Boolean MemoryBlock::StoreUnsignedWord(phi::usize address, phi::u32 value)
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        phi::usize   start_address = address - m_StartingAddress;
        std::uint8_t first_byte    = (value.get() & 0b11111111'00000000'00000000'00000000) >> 24;
        std::uint8_t second_byte   = (value.get() & 0b00000000'11111111'00000000'00000000) >> 16;
        std::uint8_t third_byte    = (value.get() & 0b00000000'00000000'11111111'00000000) >> 8;
        std::uint8_t fourth_byte   = value.get() & 0b00000000'00000000'00000000'11111111;

        m_Values.at(start_address.get()).unsigned_value        = first_byte;
        m_Values.at((start_address + 1u).get()).unsigned_value = second_byte;
        m_Values.at((start_address + 2u).get()).unsigned_value = third_byte;
        m_Values.at((start_address + 3u).get()).unsigned_value = fourth_byte;

        return true;
    }

    phi::Boolean MemoryBlock::StoreFloat(phi::usize address, phi::f32 value)
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        float         value_raw  = value.get();
        std::uint32_t value_bits = *reinterpret_cast<std::uint32_t*>(&value_raw);

        phi::usize   start_address = address - m_StartingAddress;
        std::uint8_t first_byte    = (value_bits & 0b11111111'00000000'00000000'00000000) >> 24;
        std::uint8_t second_byte   = (value_bits & 0b00000000'11111111'00000000'00000000) >> 16;
        std::uint8_t third_byte    = (value_bits & 0b00000000'00000000'11111111'00000000) >> 8;
        std::uint8_t fourth_byte   = value_bits & 0b00000000'00000000'00000000'11111111;

        m_Values.at(start_address.get()).unsigned_value        = first_byte;
        m_Values.at((start_address + 1u).get()).unsigned_value = second_byte;
        m_Values.at((start_address + 2u).get()).unsigned_value = third_byte;
        m_Values.at((start_address + 3u).get()).unsigned_value = fourth_byte;

        return true;
    }

    phi::Boolean MemoryBlock::StoreDouble(phi::usize address, phi::f64 value)
    {
        if (!IsAddressValid(address, 8u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        double value_raw = value.get();

        std::uint64_t value_bits = *reinterpret_cast<std::uint64_t*>(&value_raw);

        std::uint32_t first_bits = value_bits & 0xFFFFFFFF;
        std::uint32_t last_bits  = value_bits >> 32;

        float first_value = *reinterpret_cast<float*>(&first_bits);
        float last_value  = *reinterpret_cast<float*>(&last_bits);

        StoreFloat(address, first_value);
        StoreFloat(address + 4u, last_value);

        return true;
    }

    phi::Boolean MemoryBlock::IsAddressValid(phi::usize address, phi::usize size) const
    {
        return address >= m_StartingAddress &&
               (address + size) <= (m_StartingAddress + m_Values.size());
    }

    void MemoryBlock::Clear()
    {
        for (auto& val : m_Values)
        {
            val.signed_value = 0;
        }
    }
} // namespace dlx
