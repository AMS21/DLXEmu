#include "DLX/FloatRegister.hpp"

#include <phi/compiler_support/warning.hpp>

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

namespace dlx
{
    void FloatRegister::SetValue(const phi::f32 val) noexcept
    {
        m_Value = val;
    }

    PHI_ATTRIBUTE_PURE phi::f32 FloatRegister::GetValue() const noexcept
    {
        return m_Value;
    }
} // namespace dlx
