#pragma once

#include <Phi/Core/Types.hpp>
#include <cstdint>
#include <string>

namespace dlx
{
    class IntRegister
    {
    public:
        IntRegister();

        void SetSignedValue(phi::i32 val);

        void SetUnsignedValue(phi::u32 val);

        phi::i32 GetSignedValue() const;

        phi::u32 GetUnsignedValue() const;

        [[nodiscard]] phi::Boolean IsReadOnly() const;

        void SetReadOnly(phi::Boolean read_only);

    protected:
        union
        {
            phi::i32 m_ValueSigned;
            phi::u32 m_ValueUnsigned;
        };

        phi::Boolean m_IsReadOnly;
    };
} // namespace dlx
