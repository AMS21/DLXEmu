#include "DLX/StatusRegister.hpp"

namespace dlx
{
    void StatusRegister::SetStatus(phi::Boolean value)
    {
        m_Value = value;
    }

    phi::Boolean StatusRegister::Get() const
    {
        return m_Value;
    }
} // namespace dlx
