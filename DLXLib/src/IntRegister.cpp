#include "DLX/IntRegister.hpp"

#include <phi/compiler_support/warning.hpp>

PHI_MSVC_SUPPRESS_WARNING(4582)

namespace dlx
{
    IntRegister::IntRegister() noexcept
        : m_ValueSigned{0}
        , m_IsReadOnly{false}
    {}

    void IntRegister::SetSignedValue(phi::i32 val) noexcept
    {
        m_ValueSigned = val;
    }

    void IntRegister::SetUnsignedValue(phi::u32 val) noexcept
    {
        m_ValueUnsigned = val;
    }

    phi::i32 IntRegister::GetSignedValue() const noexcept
    {
        return m_ValueSigned;
    }

    phi::u32 IntRegister::GetUnsignedValue() const noexcept
    {
        return m_ValueUnsigned;
    }

    phi::boolean IntRegister::IsReadOnly() const noexcept
    {
        return m_IsReadOnly;
    }

    void IntRegister::SetReadOnly(phi::boolean read_only) noexcept
    {
        m_IsReadOnly = read_only;
    }
} // namespace dlx
