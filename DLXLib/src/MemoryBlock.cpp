#include "DLX/MemoryBlock.hpp"

#include "DLX/Logger.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/types.hpp>
#include <cstdint>

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=const")
PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

namespace dlx
{
    MemoryBlock::MemoryBlock(phi::usize start_address, phi::usize starting_size) noexcept
        : m_StartingAddress(start_address)
    {
        m_Values.resize(starting_size.unsafe());
    }

    phi::optional<phi::i8> MemoryBlock::LoadByte(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();

        return m_Values[raw_address].signed_value;
    }

    phi::optional<phi::u8> MemoryBlock::LoadUnsignedByte(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();
        return m_Values[raw_address].unsigned_value;
    }

    phi::optional<phi::i16> MemoryBlock::LoadHalfWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();

        if (!IsAddressAlignedCorrectly(raw_address, 2u))
        {
            DLX_ERROR("Address {} is misaligned", address.unsafe());
            return {};
        }

        return *reinterpret_cast<const std::int16_t*>(&m_Values[raw_address].signed_value);
    }

    phi::optional<phi::u16> MemoryBlock::LoadUnsignedHalfWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();

        if (!IsAddressAlignedCorrectly(raw_address, 2u))
        {
            DLX_ERROR("Address {} is misaligned", address.unsafe());
            return {};
        }

        return *reinterpret_cast<const std::uint16_t*>(&m_Values[raw_address].unsigned_value);
    }

    phi::optional<phi::i32> MemoryBlock::LoadWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();

        if (!IsAddressAlignedCorrectly(raw_address, 4u))
        {
            DLX_ERROR("Address {} is misaligned", address.unsafe());
            return {};
        }

        return *reinterpret_cast<const std::int32_t*>(&m_Values[raw_address].signed_value);
    }

    phi::optional<phi::u32> MemoryBlock::LoadUnsignedWord(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();

        if (!IsAddressAlignedCorrectly(raw_address, 4u))
        {
            DLX_ERROR("Address {} is misaligned", address.unsafe());
            return {};
        }

        return *reinterpret_cast<const std::uint32_t*>(&m_Values[raw_address].unsigned_value);
    }

    phi::optional<phi::f32> MemoryBlock::LoadFloat(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();

        if (!IsAddressAlignedCorrectly(raw_address, sizeof(phi::f32)))
        {
            DLX_ERROR("Address {} is misaligned", address.unsafe());
            return {};
        }

        return *reinterpret_cast<const float*>(&m_Values[raw_address].signed_value);
    }

    phi::optional<phi::f64> MemoryBlock::LoadDouble(phi::usize address) const noexcept
    {
        if (!IsAddressValid(address, 8u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return {};
        }

        const phi::size_t raw_address = (address - m_StartingAddress).unsafe();

        if (!IsAddressAlignedCorrectly(raw_address, sizeof(phi::f64)))
        {
            DLX_ERROR("Address {} is misaligned", address.unsafe());
            return {};
        }

        return *reinterpret_cast<const double*>(&m_Values[(raw_address)].signed_value);
    }

    phi::boolean MemoryBlock::StoreByte(phi::usize address, phi::i8 value) noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        m_Values[(address - m_StartingAddress).unsafe()].signed_value = value.unsafe();
        return true;
    }

    phi::boolean MemoryBlock::StoreUnsignedByte(phi::usize address, phi::u8 value) noexcept
    {
        if (!IsAddressValid(address, 1u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        m_Values[(address - m_StartingAddress).unsafe()].unsigned_value = value.unsafe();
        return true;
    }

    phi::boolean MemoryBlock::StoreHalfWord(phi::usize address, phi::i16 value) noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        phi::size_t index = (address - m_StartingAddress).unsafe();
        *reinterpret_cast<std::int16_t*>(&m_Values[index].signed_value) = value.unsafe();

        return true;
    }

    phi::boolean MemoryBlock::StoreUnsignedHalfWord(phi::usize address, phi::u16 value) noexcept
    {
        if (!IsAddressValid(address, 2u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        phi::size_t index = (address - m_StartingAddress).unsafe();
        *reinterpret_cast<std::uint16_t*>(&m_Values[index].unsigned_value) = value.unsafe();

        return true;
    }

    phi::boolean MemoryBlock::StoreWord(phi::usize address, phi::i32 value) noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        phi::size_t index = (address - m_StartingAddress).unsafe();
        *reinterpret_cast<std::int32_t*>(&m_Values[index].signed_value) = value.unsafe();

        return true;
    }

    phi::boolean MemoryBlock::StoreUnsignedWord(phi::usize address, phi::u32 value) noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        phi::size_t index = (address - m_StartingAddress).unsafe();
        *reinterpret_cast<std::uint32_t*>(&m_Values[index].unsigned_value) = value.unsafe();

        return true;
    }

    phi::boolean MemoryBlock::StoreFloat(phi::usize address, phi::f32 value) noexcept
    {
        if (!IsAddressValid(address, 4u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        phi::size_t index = (address - m_StartingAddress).unsafe();
        *reinterpret_cast<float*>(&m_Values[index].signed_value) = value.unsafe();

        return true;
    }

    phi::boolean MemoryBlock::StoreDouble(phi::usize address, phi::f64 value) noexcept
    {
        if (!IsAddressValid(address, 8u))
        {
            DLX_ERROR("Address {} is out of bounds", address.unsafe());
            return false;
        }

        phi::size_t index = (address - m_StartingAddress).unsafe();
        *reinterpret_cast<double*>(&m_Values[index].signed_value) = value.unsafe();

        return true;
    }

    phi::boolean MemoryBlock::IsAddressValid(phi::usize address, phi::usize size) const noexcept
    {
        return address >= m_StartingAddress &&
               !phi::detail::will_addition_error(phi::detail::arithmetic_tag_for<phi::size_t>{},
                                                 address.unsafe(), size.unsafe()) &&
               (address + size) <= (m_StartingAddress + m_Values.size());
    }

    PHI_ATTRIBUTE_CONST phi::boolean MemoryBlock::IsAddressAlignedCorrectly(
            phi::usize address, phi::usize size) noexcept
    {
        return size != 0u && (address % size) == 0u;
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

    phi::usize MemoryBlock::GetSize() const noexcept
    {
        return m_Values.size();
    }

    void MemoryBlock::Resize(phi::usize new_size) noexcept
    {
        m_Values.resize(new_size.unsafe());
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
