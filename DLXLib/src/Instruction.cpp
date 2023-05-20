#include "DLX/Instruction.hpp"

#include "DLX/InstructionArgument.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5262)
#include <fmt/format.h>
PHI_MSVC_SUPPRESS_WARNING_POP()

namespace dlx
{
    Instruction::Instruction(const InstructionInfo& info, const phi::u64 source_line) noexcept
        : m_Info(info)
        , m_SourceLine{source_line}
    {}

    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

    void Instruction::SetArgument(phi::u8 argument_number, InstructionArgument argument) noexcept
    {
        PHI_ASSERT(argument_number < 3u);

        switch (argument_number.unsafe())
        {
            case 0u:
                m_Arg1 = argument;
                break;
            case 1u:
                m_Arg2 = argument;
                break;
            case 2u:
                m_Arg3 = argument;
                break;
#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_ASSERT_NOT_REACHED();
#endif
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()

    PHI_GCC_SUPPRESS_WARNING_PUSH()
    PHI_GCC_SUPPRESS_WARNING("-Wreturn-type")

    std::string Instruction::DebugInfo() const noexcept
    {
        switch (m_Info.GetNumberOfRequiredArguments().unsafe())
        {
            case 0:
                return fmt::format("{}", dlx::enum_name(m_Info.GetOpCode()).data());
            case 1:
                return fmt::format("{}, {}", dlx::enum_name(m_Info.GetOpCode()).data(),
                                   m_Arg1.DebugInfo());
            case 2:
                return fmt::format("{}, {}, {}", dlx::enum_name(m_Info.GetOpCode()).data(),
                                   m_Arg1.DebugInfo(), m_Arg2.DebugInfo());
            case 3:
                return fmt::format("{}, {}, {}, {}", dlx::enum_name(m_Info.GetOpCode()).data(),
                                   m_Arg1.DebugInfo(), m_Arg2.DebugInfo(), m_Arg3.DebugInfo());

#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_ASSERT_NOT_REACHED();
                break;
#endif
        }

#if !defined(DLXEMU_COVERAGE_BUILD)
        PHI_ASSERT_NOT_REACHED();
        return "Unknown";
#endif
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

    void Instruction::Execute(Processor& processor) const noexcept
    {
        m_Info.Execute(processor, m_Arg1, m_Arg2, m_Arg3);
    }

    const InstructionInfo& Instruction::GetInfo() const noexcept
    {
        return m_Info;
    }

    const phi::u64 Instruction::GetSourceLine() const noexcept
    {
        return m_SourceLine;
    }

    const InstructionArgument& Instruction::GetArg1() const noexcept
    {
        return m_Arg1;
    }

    const InstructionArgument& Instruction::GetArg2() const noexcept
    {
        return m_Arg2;
    }

    const InstructionArgument& Instruction::GetArg3() const noexcept
    {
        return m_Arg3;
    }
} // namespace dlx
