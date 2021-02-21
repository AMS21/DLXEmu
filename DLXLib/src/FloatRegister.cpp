#include "DLX/FloatRegister.hpp"

namespace dlx
{
    void FloatRegister::SetValue(const phi::f32 val)
    {
        m_Value = val;
    }

    phi::f32 FloatRegister::GetValue() const
    {
        return m_Value;
    }
} // namespace dlx
