#pragma once

#include "InstructionLibrary.hpp"
#include "RegisterNames.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/scope_ptr.hpp>

namespace dlx
{
    class InstructionArgument
    {
    public:
        struct RegisterInt
        {
            IntRegisterID register_id{IntRegisterID::None};
        };

        struct RegisterFloat
        {
            FloatRegisterID register_id{FloatRegisterID::None};
        };

        struct ImmediateValue
        {
            union
            {
                phi::i16 signed_value{static_cast<phi::int16_t>(0)};
                phi::u16 unsigned_value;
            };
        };

        struct AddressDisplacement
        {
            IntRegisterID register_id{IntRegisterID::None};
            phi::i32      displacement{0};
        };

        struct Label
        {
            phi::string_view label_name;
        };

    public:
        InstructionArgument() noexcept;

        InstructionArgument(const InstructionArgument&) = default;
        InstructionArgument(InstructionArgument&&)      = default;

        InstructionArgument& operator=(const InstructionArgument&) = default;
        InstructionArgument& operator=(InstructionArgument&&)      = default;

        [[nodiscard]] ArgumentType GetType() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

        [[nodiscard]] std::string DebugInfo() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_POP()

        [[nodiscard]] const RegisterInt&         AsRegisterInt() const noexcept;
        [[nodiscard]] const RegisterFloat&       AsRegisterFloat() const noexcept;
        [[nodiscard]] const ImmediateValue&      AsImmediateValue() const noexcept;
        [[nodiscard]] const AddressDisplacement& AsAddressDisplacement() const noexcept;
        [[nodiscard]] const Label&               AsLabel() const noexcept;

        friend InstructionArgument ConstructInstructionArgumentRegisterInt(
                IntRegisterID id) noexcept;

        friend InstructionArgument ConstructInstructionArgumentRegisterFloat(
                FloatRegisterID id) noexcept;

        friend InstructionArgument ConstructInstructionArgumentImmediateValue(
                phi::int16_t value) noexcept;

        friend InstructionArgument ConstructInstructionArgumentAddressDisplacement(
                IntRegisterID id, phi::i32 displacement) noexcept;

        friend InstructionArgument ConstructInstructionArgumentLabel(
                phi::string_view label_name) noexcept;

    private:
        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4582) // 'x': constructor is not implicitly called

        union
        {
            RegisterInt         register_int;
            RegisterFloat       register_float;
            ImmediateValue      immediate_value;
            AddressDisplacement address_displacement;
            Label               label;
        };

        PHI_MSVC_SUPPRESS_WARNING_POP()

        ArgumentType m_Type;
    };

    phi::boolean operator==(const InstructionArgument& lhs,
                            const InstructionArgument& rhs) noexcept;

    phi::boolean operator!=(const InstructionArgument& lhs,
                            const InstructionArgument& rhs) noexcept;

    InstructionArgument ConstructInstructionArgumentRegisterInt(IntRegisterID id) noexcept;

    InstructionArgument ConstructInstructionArgumentRegisterFloat(FloatRegisterID id) noexcept;

    InstructionArgument ConstructInstructionArgumentImmediateValue(phi::int16_t value) noexcept;

    InstructionArgument ConstructInstructionArgumentAddressDisplacement(
            IntRegisterID id, phi::i32 displacement) noexcept;

    InstructionArgument ConstructInstructionArgumentLabel(phi::string_view label_name) noexcept;
} // namespace dlx
