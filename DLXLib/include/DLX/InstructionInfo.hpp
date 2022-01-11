#pragma once

#include "DLX/OpCode.hpp"
#include <Phi/Core/Assert.hpp>
#include <Phi/Core/Types.hpp>
#include <type_traits>

namespace dlx
{
    class Processor;
    class InstructionArgument;

    enum class ArgumentType
    {
        Unknown          = 0,  // Should not be used just to check for errors
        None             = 1,  // Meaning this argument is expected to be not present
        IntRegister      = 2,  // Meaning this argument is expected to be an integer register
        FloatRegister    = 4,  // Meaning this argument is expected to be an float register
        ImmediateInteger = 8,  // Meaning this argument is expected to be an immediate integer value
        Label            = 16, // Meaning this argument is expected to be an Label identifier
        AddressDisplacement = 32, // Meaning this argument is expected to be an address displacement
    };

    constexpr ArgumentType operator&(ArgumentType lhs, ArgumentType rhs) noexcept
    {
        using underlying_t = std::underlying_type_t<ArgumentType>;

        return static_cast<ArgumentType>(static_cast<underlying_t>(lhs) &
                                         static_cast<underlying_t>(rhs));
    }

    constexpr ArgumentType operator|(ArgumentType lhs, ArgumentType rhs) noexcept
    {
        using underlying_t = std::underlying_type_t<ArgumentType>;

        return static_cast<ArgumentType>(static_cast<underlying_t>(lhs) |
                                         static_cast<underlying_t>(rhs));
    }

    [[nodiscard]] constexpr phi::Boolean ArgumentTypeIncludes(ArgumentType type,
                                                              ArgumentType test) noexcept
    {
        using underlying_t = std::underlying_type_t<ArgumentType>;

        return static_cast<underlying_t>(type & test) != 0;
    }

    enum class RegisterAccessType
    {
        None,
        Signed,
        Unsigned,
        MixedSignedUnsigned,
        Float,
        Double,
        MixedFloatDouble,
        MixedFloatSigned,
        MixedDoubleSigned,
        Ignored,
    };

    using InstructionExecutor = std::add_pointer_t<void(
            Processor& processor, const InstructionArgument& arg1, const InstructionArgument& arg2,
            const InstructionArgument& arg3)>;

    // Class holding all the data and information about a specific instruction
    class InstructionInfo
    {
    public:
        constexpr InstructionInfo() noexcept
            : m_OpCode(OpCode::NONE)
            , m_Arg1Type(ArgumentType::Unknown)
            , m_Arg2Type(ArgumentType::Unknown)
            , m_Arg3Type(ArgumentType::Unknown)
            , m_RegisterAccessType(RegisterAccessType::None)
            , m_Executor(nullptr)
        {}

        constexpr InstructionInfo(OpCode opcode, ArgumentType arg1, ArgumentType arg2,
                                  ArgumentType arg3, RegisterAccessType register_access_type,
                                  InstructionExecutor executor) noexcept
            : m_OpCode(opcode)
            , m_Arg1Type(arg1)
            , m_Arg2Type(arg2)
            , m_Arg3Type(arg3)
            , m_RegisterAccessType(register_access_type)
            , m_Executor(executor)
        {}

        [[nodiscard]] constexpr OpCode GetOpCode() const noexcept
        {
            return m_OpCode;
        }

        [[nodiscard]] constexpr ArgumentType GetArgumentType(phi::u8 index) const noexcept
        {
            PHI_ASSERT(index < 3u);

            switch (index.get())
            {
                case 0:
                    return m_Arg1Type;

                case 1:
                    return m_Arg2Type;

                case 2:
                    return m_Arg3Type;

#if !defined(DLXEMU_COVERAGE_BUILD)
                default:
                    PHI_ASSERT_NOT_REACHED();
                    return m_Arg1Type;
#endif
            }
        }

        [[nodiscard]] constexpr phi::u8 GetNumberOfRequiredArguments() const noexcept
        {
            phi::u8 number_of_argument_required = static_cast<std::uint8_t>(0u);

            // Count the number of argument we need
            if (m_Arg1Type != ArgumentType::None)
            {
                number_of_argument_required += static_cast<std::uint8_t>(1u);
            }
            if (m_Arg2Type != ArgumentType::None)
            {
                number_of_argument_required += static_cast<std::uint8_t>(1u);
            }
            if (m_Arg3Type != ArgumentType::None)
            {
                number_of_argument_required += static_cast<std::uint8_t>(1u);
            }

            return number_of_argument_required;
        }

        [[nodiscard]] constexpr RegisterAccessType GetRegisterAccessType() const noexcept
        {
            return m_RegisterAccessType;
        }

        [[nodiscard]] constexpr InstructionExecutor GetExecutor() const noexcept
        {
            return m_Executor;
        }

        void Execute(Processor& processor, const InstructionArgument& arg1,
                     const InstructionArgument& arg2,
                     const InstructionArgument& arg3) const noexcept;

    private:
        OpCode              m_OpCode;
        ArgumentType        m_Arg1Type;
        ArgumentType        m_Arg2Type;
        ArgumentType        m_Arg3Type;
        RegisterAccessType  m_RegisterAccessType;
        InstructionExecutor m_Executor;
    };
} // namespace dlx
