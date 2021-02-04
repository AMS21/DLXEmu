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

        struct ImmediateValue
        {
            union
            {
                phi::i32 signed_value{0};
                phi::u32 unsigned_value;
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

        ArgumentType m_Type;

        [[nodiscard]] const RegisterInt&         AsRegisterInt() const;
        [[nodiscard]] const ImmediateValue&      AsImmediateValue() const;
        [[nodiscard]] const AddressDisplacement& AsAddressDisplacement() const;
        [[nodiscard]] const Label&               AsLabel() const;

        friend InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id);

        friend InstructionArg ConstructInstructionArgImmediateValue(phi::i32 value);

        friend InstructionArg ConstructInstructionArgAddressDisplacement(IntRegisterID id,
                                                                         phi::i32 displacement);

        friend InstructionArg ConstructInstructionArgLabel(std::string_view label_name);

    private:
        union
        {
            RegisterInt         register_int;
            ImmediateValue      immediate_value;
            AddressDisplacement address_displacement;
            Label               label;
        };
    };

    phi::Boolean operator==(const InstructionArg& lhs, const InstructionArg& rhs);

    phi::Boolean operator!=(const InstructionArg& lhs, const InstructionArg& rhs);

    InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id);

    InstructionArg ConstructInstructionArgImmediateValue(phi::i32 value);

    InstructionArg ConstructInstructionArgAddressDisplacement(IntRegisterID id,
                                                              phi::i32      displacement);

    InstructionArg ConstructInstructionArgLabel(std::string_view label_name);
} // namespace dlx
