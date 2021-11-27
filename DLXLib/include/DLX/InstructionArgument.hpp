#pragma once

#include "InstructionLibrary.hpp"
#include "RegisterNames.hpp"
#include <Phi/Core/ScopePtr.hpp>
#include <string_view>

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
                phi::i16 signed_value{static_cast<std::int16_t>(0)};
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
            std::string_view label_name;
        };

    public:
        InstructionArgument() noexcept;

        InstructionArgument(const InstructionArgument&) = default;
        InstructionArgument(InstructionArgument&&)      = default;

        InstructionArgument& operator=(const InstructionArgument&) = default;
        InstructionArgument& operator=(InstructionArgument&&) = default;

        [[nodiscard]] ArgumentType GetType() const noexcept;

        [[nodiscard]] std::string DebugInfo() const noexcept;

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
                std::int16_t value) noexcept;

        friend InstructionArgument ConstructInstructionArgumentAddressDisplacement(
                IntRegisterID id, phi::i32 displacement) noexcept;

        friend InstructionArgument ConstructInstructionArgumentLabel(
                std::string_view label_name) noexcept;

    private:
        union
        {
            RegisterInt         register_int;
            RegisterFloat       register_float;
            ImmediateValue      immediate_value;
            AddressDisplacement address_displacement;
            Label               label;
        };

        ArgumentType m_Type;
    };

    phi::Boolean operator==(const InstructionArgument& lhs,
                            const InstructionArgument& rhs) noexcept;

    phi::Boolean operator!=(const InstructionArgument& lhs,
                            const InstructionArgument& rhs) noexcept;

    InstructionArgument ConstructInstructionArgumentRegisterInt(IntRegisterID id) noexcept;

    InstructionArgument ConstructInstructionArgumentRegisterFloat(FloatRegisterID id) noexcept;

    InstructionArgument ConstructInstructionArgumentImmediateValue(std::int16_t value) noexcept;

    InstructionArgument ConstructInstructionArgumentAddressDisplacement(
            IntRegisterID id, phi::i32 displacement) noexcept;

    InstructionArgument ConstructInstructionArgumentLabel(std::string_view label_name) noexcept;
} // namespace dlx
