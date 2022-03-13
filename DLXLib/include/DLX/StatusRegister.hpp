#pragma once

#include <phi/core/boolean.hpp>

namespace dlx
{
    class StatusRegister
    {
    public:
        void SetStatus(phi::boolean value) noexcept;

        [[nodiscard]] phi::boolean Get() const noexcept;

    private:
        phi::boolean m_Value{false};
    };
} // namespace dlx
