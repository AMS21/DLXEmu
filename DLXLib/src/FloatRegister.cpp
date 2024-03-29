#include "DLX/FloatRegister.hpp"

#include <phi/compiler_support/warning.hpp>

namespace dlx
{
    void FloatRegister::SetValue(const phi::f32 val) noexcept
    {
        m_Value = val;
    }

    phi::f32 FloatRegister::GetValue() const noexcept
    {
        return m_Value;
    }
} // namespace dlx
