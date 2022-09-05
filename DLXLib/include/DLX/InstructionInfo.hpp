#pragma once

#include "DLX/OpCode.hpp"
#include "DLX/StatusRegister.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/type_traits/to_underlying.hpp>

PHI_GCC_SUPPRESS_WARNING("-Wstrict-overflow")

#include "DLX/EnumName.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/types.hpp>
#include <phi/type_traits/underlying_type.hpp>

namespace dlx
{
    class Processor;
    class InstructionArgument;

#define DLX_ENUM_ARGUMENT_TYPE                                                                     \
    DLX_ENUM_ARGUMENT_TYPE_IMPL(Unknown, 0)                                                        \
    DLX_ENUM_ARGUMENT_TYPE_IMPL(None, 1)                                                           \
    DLX_ENUM_ARGUMENT_TYPE_IMPL(IntRegister, 2)                                                    \
    DLX_ENUM_ARGUMENT_TYPE_IMPL(FloatRegister, 4)                                                  \
    DLX_ENUM_ARGUMENT_TYPE_IMPL(ImmediateInteger, 8)                                               \
    DLX_ENUM_ARGUMENT_TYPE_IMPL(Label, 16)                                                         \
    DLX_ENUM_ARGUMENT_TYPE_IMPL(AddressDisplacement, 32)

    enum class ArgumentType : phi::uint8_t
    {
#define DLX_ENUM_ARGUMENT_TYPE_IMPL(name, value) name = value,
        DLX_ENUM_ARGUMENT_TYPE
#undef DLX_ENUM_ARGUMENT_TYPE_IMPL
    };

    constexpr ArgumentType operator&(ArgumentType lhs, ArgumentType rhs) noexcept
    {
        using underlying_t = phi::underlying_type_t<ArgumentType>;

        return static_cast<ArgumentType>(static_cast<underlying_t>(lhs) &
                                         static_cast<underlying_t>(rhs));
    }

    constexpr ArgumentType operator|(ArgumentType lhs, ArgumentType rhs) noexcept
    {
        using underlying_t = phi::underlying_type_t<ArgumentType>;

        return static_cast<ArgumentType>(static_cast<underlying_t>(lhs) |
                                         static_cast<underlying_t>(rhs));
    }

    [[nodiscard]] constexpr phi::boolean ArgumentTypeIncludes(ArgumentType type,
                                                              ArgumentType test) noexcept
    {
        using underlying_t = std::underlying_type_t<ArgumentType>;

        return static_cast<underlying_t>(type & test) != 0;
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wreturn-type")

    template <>
    [[nodiscard]] constexpr std::string_view enum_name<ArgumentType>(ArgumentType value) noexcept
    {
        switch (phi::to_underlying(value))
        {
#define DLX_ENUM_ARGUMENT_TYPE_IMPL(name, enum_value)                                              \
    case enum_value:                                                                               \
        return #name;

            DLX_ENUM_ARGUMENT_TYPE

#undef DLX_ENUM_ARGUMENT_TYPE_IMPL

            // Explicitly handle the combine cases
            case phi::to_underlying(ArgumentType::ImmediateInteger |
                                    ArgumentType::AddressDisplacement):
                return "ImmediateInteger/AddressDisplacement";

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

#define DLX_ENUM_REGISTER_ACCESS_TYPE                                                              \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(None)                                                       \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(Signed)                                                     \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(Unsigned)                                                   \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(MixedSignedUnsigned)                                        \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(Float)                                                      \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(Double)                                                     \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(MixedFloatDouble)                                           \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(MixedFloatSigned)                                           \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(MixedDoubleSigned)                                          \
    DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(Ignored)

    enum class RegisterAccessType : phi::uint8_t
    {
#define DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(name) name,
        DLX_ENUM_REGISTER_ACCESS_TYPE
#undef DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL
    };

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wreturn-type")

    template <>
    [[nodiscard]] constexpr std::string_view enum_name<RegisterAccessType>(
            RegisterAccessType value) noexcept
    {
        switch (value)
        {
#define DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL(name)                                                   \
    case RegisterAccessType::name:                                                                 \
        return #name;

            DLX_ENUM_REGISTER_ACCESS_TYPE
#undef DLX_ENUM_REGISTER_ACCESS_TYPE_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

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

        PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
        PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wreturn-type")

        [[nodiscard]] constexpr ArgumentType GetArgumentType(phi::u8 index) const noexcept
        {
            PHI_ASSERT(index < 3u);

            switch (index.unsafe())
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

        PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

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
