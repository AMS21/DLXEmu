#pragma once

#include "Phi/Core/Boolean.hpp"

namespace dlx
{
    class StatusRegister
    {
    public:
        void SetStatus(phi::Boolean value) noexcept;

        [[nodiscard]] phi::Boolean Get() const noexcept;

    private:
        phi::Boolean m_Value{false};
    };
} // namespace dlx
