#include "DLX/IntRegister.hpp"

namespace dlx
{
    IntRegister::IntRegister()
        : m_ValueSigned{0}
        , m_IsReadOnly{false}
    {}

    void IntRegister::SetSignedValue(phi::i32 val)
    {
        m_ValueSigned = val;
    }

    void IntRegister::SetUnsignedValue(phi::u32 val)
    {
        m_ValueUnsigned = val;
    }

    phi::i32 IntRegister::GetSignedValue() const
    {
        return m_ValueSigned;
    }

    phi::u32 IntRegister::GetUnsignedValue() const
    {
        return m_ValueUnsigned;
    }

    phi::Boolean IntRegister::IsReadOnly() const
    {
        return m_IsReadOnly;
    }

    void IntRegister::SetReadOnly(phi::Boolean read_only)
    {
        m_IsReadOnly = read_only;
    }
} // namespace dlx
