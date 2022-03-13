#include "DLX/StatusRegister.hpp"

namespace dlx
{
    void StatusRegister::SetStatus(phi::boolean value) noexcept
    {
        m_Value = value;
    }

    phi::boolean StatusRegister::Get() const noexcept
    {
        return m_Value;
    }
} // namespace dlx
