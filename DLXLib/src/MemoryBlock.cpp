#include "DLX/MemoryBlock.hpp"
#include "Phi/Core/Boolean.hpp"
#include "Phi/Core/Log.hpp"
#include <cstdint>

namespace dlx
{
    MemoryBlock::MemoryBlock(phi::usize start_address, phi::usize starting_size) noexcept
        : m_StartingAddress(start_address)
    {
        m_Values.resize(starting_size.get());
    }

    std::optional<phi::i8> MemoryBlock::LoadByte(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return m_Values[index].signed_value;
    }

    std::optional<phi::u8> MemoryBlock::LoadUnsignedByte(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return m_Values[index].unsigned_value;
    }

    std::optional<phi::i16> MemoryBlock::LoadHalfWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }
        if (!IsAddressAlignedCorrectly(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is misaligned", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return *reinterpret_cast<const std::int16_t*>(&m_Values[index].signed_value);
    }

    std::optional<phi::u16> MemoryBlock::LoadUnsignedHalfWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }
        if (!IsAddressAlignedCorrectly(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is misaligned", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return *reinterpret_cast<const std::uint16_t*>(&m_Values[index].unsigned_value);
    }

    std::optional<phi::i32> MemoryBlock::LoadWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }
        if (!IsAddressAlignedCorrectly(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is misaligned", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return *reinterpret_cast<const std::int32_t*>(&m_Values[index].signed_value);
    }

    std::optional<phi::u32> MemoryBlock::LoadUnsignedWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }
        if (!IsAddressAlignedCorrectly(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is misaligned", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return *reinterpret_cast<const std::uint32_t*>(&m_Values[index].unsigned_value);
    }

    std::optional<phi::f32> MemoryBlock::LoadFloat(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return *reinterpret_cast<const float*>(&m_Values[index].signed_value);
    }

    std::optional<phi::f64> MemoryBlock::LoadDouble(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 8u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return {};
        }

        std::size_t index = (address - m_StartingAddress).get();
        return *reinterpret_cast<const double*>(&m_Values[(index)].signed_value);
    }

    phi::Boolean MemoryBlock::StoreByte(phi::usize address, phi::i8 value) noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        m_Values[(address - m_StartingAddress).get()].signed_value = value.get();
        return true;
    }

    phi::Boolean MemoryBlock::StoreUnsignedByte(phi::usize address, phi::u8 value) noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        m_Values[(address - m_StartingAddress).get()].unsigned_value = value.get();
        return true;
    }

    phi::Boolean MemoryBlock::StoreHalfWord(phi::usize address, phi::i16 value) noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        std::size_t index = (address - m_StartingAddress).get();
        *reinterpret_cast<std::int16_t*>(&m_Values[index].signed_value) = value.get();

        return true;
    }

    phi::Boolean MemoryBlock::StoreUnsignedHalfWord(phi::usize address, phi::u16 value) noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        std::size_t index = (address - m_StartingAddress).get();
        *reinterpret_cast<std::uint16_t*>(&m_Values[index].unsigned_value) = value.get();

        return true;
    }

    phi::Boolean MemoryBlock::StoreWord(phi::usize address, phi::i32 value) noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        std::size_t index = (address - m_StartingAddress).get();
        *reinterpret_cast<std::int32_t*>(&m_Values[index].signed_value) = value.get();

        return true;
    }

    phi::Boolean MemoryBlock::StoreUnsignedWord(phi::usize address, phi::u32 value) noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        std::size_t index = (address - m_StartingAddress).get();
        *reinterpret_cast<std::uint32_t*>(&m_Values[index].unsigned_value) = value.get();

        return true;
    }

    phi::Boolean MemoryBlock::StoreFloat(phi::usize address, phi::f32 value) noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        std::size_t index = (address - m_StartingAddress).get();
        *reinterpret_cast<float*>(&m_Values[index].signed_value) = value.get();

        return true;
    }

    phi::Boolean MemoryBlock::StoreDouble(phi::usize address, phi::f64 value) noexcept
    {
        if (!IsAddressValid(address, 8u))
        {
            PHI_LOG_ERROR("Address {} is out of bounds", address.get());
            return false;
        }

        std::size_t index = (address - m_StartingAddress).get();
        *reinterpret_cast<double*>(&m_Values[index].signed_value) = value.get();

        return true;
    }

    phi::Boolean MemoryBlock::IsAddressValid(phi::usize address, phi::usize size) const noexcept
    {
        return address >= m_StartingAddress &&
               (address + size) <= (m_StartingAddress + m_Values.size());
    }

    phi::Boolean MemoryBlock::IsAddressAlignedCorrectly(phi::usize address,
                                                        phi::usize size) noexcept
    {
        return (address % size) == 0u;
    }

    void MemoryBlock::Clear() noexcept
    {
        for (auto& val : m_Values)
        {
            val.signed_value = 0;
        }
    }

    phi::usize MemoryBlock::GetStartingAddress() const noexcept
    {
        return m_StartingAddress;
    }

    void MemoryBlock::SetStartingAddress(phi::usize new_starting_address) noexcept
    {
        m_StartingAddress = new_starting_address;
    }

    std::vector<MemoryBlock::MemoryByte>& MemoryBlock::GetRawMemory() noexcept
    {
        return m_Values;
    }

    const std::vector<MemoryBlock::MemoryByte>& MemoryBlock::GetRawMemory() const noexcept
    {
        return m_Values;
    }
} // namespace dlx
