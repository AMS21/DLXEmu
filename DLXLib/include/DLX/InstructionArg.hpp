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
        InstructionArg();

        InstructionArg(const InstructionArg&) = default;
        InstructionArg(InstructionArg&&)      = default;

        InstructionArg& operator=(const InstructionArg&) = default;
        InstructionArg& operator=(InstructionArg&&) = default;

        ArgumentType GetType() const;

        std::string DebugInfo() const;

        [[nodiscard]] const RegisterInt&         AsRegisterInt() const;
        [[nodiscard]] const RegisterFloat&       AsRegisterFloat() const;
        [[nodiscard]] const ImmediateValue&      AsImmediateValue() const;
        [[nodiscard]] const AddressDisplacement& AsAddressDisplacement() const;
        [[nodiscard]] const Label&               AsLabel() const;

        friend InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id);

        friend InstructionArg ConstructInstructionArgRegisterFloat(FloatRegisterID id);

        friend InstructionArg ConstructInstructionArgImmediateValue(std::int16_t value);

        friend InstructionArg ConstructInstructionArgAddressDisplacement(IntRegisterID id,
                                                                         phi::i32 displacement);

        friend InstructionArg ConstructInstructionArgLabel(std::string_view label_name);

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

    phi::Boolean operator==(const InstructionArg& lhs, const InstructionArg& rhs);

    phi::Boolean operator!=(const InstructionArg& lhs, const InstructionArg& rhs);

    InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id);

    InstructionArg ConstructInstructionArgRegisterFloat(FloatRegisterID id);

    InstructionArg ConstructInstructionArgImmediateValue(std::int16_t value);

    InstructionArg ConstructInstructionArgAddressDisplacement(IntRegisterID id,
                                                              phi::i32      displacement);

    InstructionArg ConstructInstructionArgLabel(std::string_view label_name);
} // namespace dlx
