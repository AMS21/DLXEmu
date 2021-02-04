#include "DLX/MemoryBlock.hpp"

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
