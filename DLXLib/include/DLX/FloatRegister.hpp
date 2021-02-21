#pragma once

#include <Phi/Core/Types.hpp>

namespace dlx
{
    class FloatRegister
    {
    public:
        void SetValue(phi::f32 val);

        [[nodiscard]] phi::f32 GetValue() const;

    private:
        phi::f32 m_Value{0.0f};
    };
} // namespace dlx
