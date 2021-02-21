#pragma once

#include "Phi/Core/Boolean.hpp"

namespace dlx
{
    class StatusRegister
    {
    public:
        void SetStatus(phi::Boolean value);

        [[nodiscard]] phi::Boolean Get() const;

    private:
        phi::Boolean m_Value{false};
    };
} // namespace dlx
