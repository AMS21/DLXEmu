#include "DLX/InstructionArg.hpp"

namespace dlx
{
    InstructionArg::InstructionArg()
        : m_Type(ArgumentType::None)
        , address_displacement()
    {}

    ArgumentType InstructionArg::GetType() const
    {
        return m_Type;
    }

    std::string InstructionArg::DebugInfo() const
    {
        return "InstructionArgument";
    }

    const InstructionArg::RegisterInt& InstructionArg::AsRegisterInt() const
    {
        PHI_ASSERT(m_Type == ArgumentType::IntRegister);

        return register_int;
    }

    const InstructionArg::RegisterFloat& InstructionArg::AsRegisterFloat() const
    {
        PHI_ASSERT(m_Type == ArgumentType::FloatRegister);

        return register_float;
    }

    const InstructionArg::ImmediateValue& InstructionArg::AsImmediateValue() const
    {
        PHI_ASSERT(m_Type == ArgumentType::ImmediateInteger);

        return immediate_value;
    }

    const InstructionArg::AddressDisplacement& InstructionArg::AsAddressDisplacement() const
    {
        PHI_ASSERT(m_Type == ArgumentType::AddressDisplacement);

        return address_displacement;
    }

    const InstructionArg::Label& InstructionArg::AsLabel() const
    {
        PHI_ASSERT(m_Type == ArgumentType::Label);

        return label;
    }

    phi::Boolean operator==(const InstructionArg& lhs, const InstructionArg& rhs)
    {
        if (lhs.GetType() != rhs.GetType())
        {
            return false;
        }

        switch (lhs.GetType())
        {
            case ArgumentType::AddressDisplacement:
                return (lhs.AsAddressDisplacement().displacement ==
                        rhs.AsAddressDisplacement().displacement) &&
                       (lhs.AsAddressDisplacement().register_id ==
                        rhs.AsAddressDisplacement().register_id);
            case ArgumentType::ImmediateInteger:
                return lhs.AsImmediateValue().signed_value == rhs.AsImmediateValue().signed_value;
            case ArgumentType::IntRegister:
                return lhs.AsRegisterInt().register_id == rhs.AsRegisterInt().register_id;
            case ArgumentType::FloatRegister:
                return lhs.AsRegisterFloat().register_id == rhs.AsRegisterFloat().register_id;
            case ArgumentType::Label:
                return lhs.AsLabel().label_name == rhs.AsLabel().label_name;
            case ArgumentType::None:
                return true;
            default:
                PHI_ASSERT_NOT_REACHED();
                break;
        }

        PHI_ASSERT_NOT_REACHED();
        return false;
    }

    phi::Boolean operator!=(const InstructionArg& lhs, const InstructionArg& rhs)
    {
        return !(lhs == rhs);
    }

    InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id)
    {
        InstructionArg arg;
        arg.m_Type                   = ArgumentType::IntRegister;
        arg.register_int.register_id = id;
        return arg;
    }

    InstructionArg ConstructInstructionArgRegisterFloat(FloatRegisterID id)
    {
        InstructionArg arg;
        arg.m_Type                     = ArgumentType::FloatRegister;
        arg.register_float.register_id = id;
        return arg;
    }

    InstructionArg ConstructInstructionArgImmediateValue(phi::i32 value)
    {
        InstructionArg arg;
        arg.m_Type                       = ArgumentType::ImmediateInteger;
        arg.immediate_value.signed_value = value;
        return arg;
    }

    InstructionArg ConstructInstructionArgAddressDisplacement(IntRegisterID id,
                                                              phi::i32      displacement)
    {
        InstructionArg arg;
        arg.m_Type                            = ArgumentType::AddressDisplacement;
        arg.address_displacement.register_id  = id;
        arg.address_displacement.displacement = displacement;
        return arg;
    }

    InstructionArg ConstructInstructionArgLabel(std::string_view label_name)
    {
        InstructionArg arg;
        arg.m_Type           = ArgumentType::Label;
        arg.label.label_name = label_name;
        return arg;
    }
} // namespace dlx
