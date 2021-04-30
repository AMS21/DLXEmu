#pragma once

#include "InstructionLibrary.hpp"
#include "RegisterNames.hpp"
#include <Phi/Core/ScopePtr.hpp>
#include <string_view>

namespace dlx
{
    class InstructionArg
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
        InstructionArg() noexcept;

        InstructionArg(const InstructionArg&) = default;
        InstructionArg(InstructionArg&&)      = default;

        InstructionArg& operator=(const InstructionArg&) = default;
        InstructionArg& operator=(InstructionArg&&) = default;

        ArgumentType GetType() const noexcept;

        std::string DebugInfo() const noexcept;

        [[nodiscard]] const RegisterInt&         AsRegisterInt() const noexcept;
        [[nodiscard]] const RegisterFloat&       AsRegisterFloat() const noexcept;
        [[nodiscard]] const ImmediateValue&      AsImmediateValue() const noexcept;
        [[nodiscard]] const AddressDisplacement& AsAddressDisplacement() const noexcept;
        [[nodiscard]] const Label&               AsLabel() const noexcept;

        friend InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id) noexcept;

        friend InstructionArg ConstructInstructionArgRegisterFloat(FloatRegisterID id) noexcept;

        friend InstructionArg ConstructInstructionArgImmediateValue(std::int16_t value) noexcept;

        friend InstructionArg ConstructInstructionArgAddressDisplacement(
                IntRegisterID id, phi::i32 displacement) noexcept;

        friend InstructionArg ConstructInstructionArgLabel(std::string_view label_name) noexcept;

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

    phi::Boolean operator==(const InstructionArg& lhs, const InstructionArg& rhs) noexcept;

    phi::Boolean operator!=(const InstructionArg& lhs, const InstructionArg& rhs) noexcept;

    InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id) noexcept;

    InstructionArg ConstructInstructionArgRegisterFloat(FloatRegisterID id) noexcept;

    InstructionArg ConstructInstructionArgImmediateValue(std::int16_t value) noexcept;

    InstructionArg ConstructInstructionArgAddressDisplacement(IntRegisterID id,
                                                              phi::i32      displacement) noexcept;

    InstructionArg ConstructInstructionArgLabel(std::string_view label_name) noexcept;
} // namespace dlx
