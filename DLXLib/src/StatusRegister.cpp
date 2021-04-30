#include "DLX/StatusRegister.hpp"

namespace dlx
{
    void StatusRegister::SetStatus(phi::Boolean value) noexcept
    {
        m_Value = value;
    }

    phi::Boolean StatusRegister::Get() const noexcept
    {
        return m_Value;
    }
} // namespace dlx
