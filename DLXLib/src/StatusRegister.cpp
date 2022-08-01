#include "DLX/StatusRegister.hpp"

#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

namespace dlx
{
    void StatusRegister::SetStatus(phi::boolean value) noexcept
    {
        m_Value = value;
    }

    PHI_ATTRIBUTE_CONST phi::boolean StatusRegister::Get() const noexcept
    {
        return m_Value;
    }
} // namespace dlx
