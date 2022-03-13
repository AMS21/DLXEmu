#pragma once

#include <phi/core/types.hpp>

namespace dlx
{
    class FloatRegister
    {
    public:
        void SetValue(phi::f32 val) noexcept;

        [[nodiscard]] phi::f32 GetValue() const noexcept;

    private:
        phi::f32 m_Value{0.0f};
    };
} // namespace dlx
