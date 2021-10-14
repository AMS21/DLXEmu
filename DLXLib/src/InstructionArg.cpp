#include "DLX/InstructionArg.hpp"

#include "DLX/InstructionInfo.hpp"
#include <Phi/Core/Assert.hpp>
#include <magic_enum.hpp>

namespace dlx
{
    InstructionArg::InstructionArg() noexcept
        : m_Type(ArgumentType::None)
        , address_displacement()
    {}

    ArgumentType InstructionArg::GetType() const noexcept
    {
        return m_Type;
    }

    std::string InstructionArg::DebugInfo() const noexcept
    {
        switch (m_Type)
        {
            case ArgumentType::None:
                return "None";

            case ArgumentType::AddressDisplacement: {
                AddressDisplacement adr = AsAddressDisplacement();
                return fmt::format("{:d}({:s})", adr.displacement.get(),
                                   magic_enum::enum_name(adr.register_id));
            }

            case ArgumentType::FloatRegister:
                return fmt::format("{:s}", magic_enum::enum_name(AsRegisterFloat().register_id));

            case ArgumentType::IntRegister:
                return fmt::format("{:s}", magic_enum::enum_name(AsRegisterInt().register_id));

            case ArgumentType::ImmediateInteger:
                return fmt::format("#{:d}", AsImmediateValue().signed_value.get());

            case ArgumentType::Label:
                return fmt::format("{:s}", AsLabel().label_name);

            default:
                PHI_ASSERT_NOT_REACHED();
                break;
        }

        PHI_ASSERT_NOT_REACHED();
        return "Unknown";
    }

    const InstructionArg::RegisterInt& InstructionArg::AsRegisterInt() const noexcept
    {
        PHI_ASSERT(m_Type == ArgumentType::IntRegister);

        return register_int;
    }

    const InstructionArg::RegisterFloat& InstructionArg::AsRegisterFloat() const noexcept
    {
        PHI_ASSERT(m_Type == ArgumentType::FloatRegister);

        return register_float;
    }

    const InstructionArg::ImmediateValue& InstructionArg::AsImmediateValue() const noexcept
    {
        PHI_ASSERT(m_Type == ArgumentType::ImmediateInteger);

        return immediate_value;
    }

    const InstructionArg::AddressDisplacement& InstructionArg::AsAddressDisplacement()
            const noexcept
    {
        PHI_ASSERT(m_Type == ArgumentType::AddressDisplacement);

        return address_displacement;
    }

    const InstructionArg::Label& InstructionArg::AsLabel() const noexcept
    {
        PHI_ASSERT(m_Type == ArgumentType::Label);

        return label;
    }

    phi::Boolean operator==(const InstructionArg& lhs, const InstructionArg& rhs) noexcept
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

    phi::Boolean operator!=(const InstructionArg& lhs, const InstructionArg& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    InstructionArg ConstructInstructionArgRegisterInt(IntRegisterID id) noexcept
    {
        InstructionArg arg;
        arg.m_Type                   = ArgumentType::IntRegister;
        arg.register_int.register_id = id;
        return arg;
    }

    InstructionArg ConstructInstructionArgRegisterFloat(FloatRegisterID id) noexcept
    {
        InstructionArg arg;
        arg.m_Type                     = ArgumentType::FloatRegister;
        arg.register_float.register_id = id;
        return arg;
    }

    InstructionArg ConstructInstructionArgImmediateValue(std::int16_t value) noexcept
    {
        InstructionArg arg;
        arg.m_Type                       = ArgumentType::ImmediateInteger;
        arg.immediate_value.signed_value = value;
        return arg;
    }

    InstructionArg ConstructInstructionArgAddressDisplacement(IntRegisterID id,
                                                              phi::i32      displacement) noexcept
    {
        InstructionArg arg;
        arg.m_Type                            = ArgumentType::AddressDisplacement;
        arg.address_displacement.register_id  = id;
        arg.address_displacement.displacement = displacement;
        return arg;
    }

    InstructionArg ConstructInstructionArgLabel(std::string_view label_name) noexcept
    {
        InstructionArg arg;
        arg.m_Type           = ArgumentType::Label;
        arg.label.label_name = label_name;
        return arg;
    }
} // namespace dlx
