#pragma once

#include <phi/core/boolean.hpp>

namespace dlx
{
    class StatusRegister
    {
    public:
        constexpr void SetStatus(phi::boolean value) noexcept
        {
            m_Value = value;
        }

        [[nodiscard]] constexpr phi::boolean Get() const noexcept
        {
            return m_Value;
        }

    private:
        phi::boolean m_Value{false};
    };
} // namespace dlx
