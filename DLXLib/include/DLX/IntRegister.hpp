#pragma once

#include <phi/compiler_support/warning.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace dlx
{
    class IntRegister
    {
    public:
        IntRegister() noexcept;

        void SetSignedValue(phi::i32 val) noexcept;

        void SetUnsignedValue(phi::u32 val) noexcept;

        [[nodiscard]] phi::i32 GetSignedValue() const noexcept;

        [[nodiscard]] phi::u32 GetUnsignedValue() const noexcept;

        [[nodiscard]] phi::boolean IsReadOnly() const noexcept;

        void SetReadOnly(phi::boolean read_only) noexcept;

    protected:
        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4582) // 'x': constructor is not implicitly called

        union
        {
            phi::i32 m_ValueSigned;
            phi::u32 m_ValueUnsigned;
        };

        PHI_MSVC_SUPPRESS_WARNING_POP()

        phi::boolean m_IsReadOnly;
    };
} // namespace dlx
