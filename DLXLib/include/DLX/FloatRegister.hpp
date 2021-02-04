#pragma once

#include <Phi/Core/Types.hpp>

namespace dlx
{
    class FloatRegister
    {
    public:
        void SetValue(phi::f32 val);

        phi::f32 GetValue();

    private:
        phi::f32 m_Value;
    };
} // namespace dlx
