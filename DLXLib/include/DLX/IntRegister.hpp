#pragma once

#include <Phi/Core/Types.hpp>
#include <cstdint>
#include <string>

namespace dlx
{
    class IntRegister
    {
    public:
        IntRegister() noexcept;

        void SetSignedValue(phi::i32 val) noexcept;

        void SetUnsignedValue(phi::u32 val) noexcept;

        phi::i32 GetSignedValue() const noexcept;

        phi::u32 GetUnsignedValue() const noexcept;

        [[nodiscard]] phi::Boolean IsReadOnly() const noexcept;

        void SetReadOnly(phi::Boolean read_only) noexcept;

    protected:
        union
        {
            phi::i32 m_ValueSigned;
            phi::u32 m_ValueUnsigned;
        };

        phi::Boolean m_IsReadOnly;
    };
} // namespace dlx
