#include "DLX/Instruction.hpp"

#include "DLX/InstructionArgument.hpp"
#include <magic_enum.hpp>
#include <phi/core/assert.hpp>
#include <spdlog/fmt/fmt.h>

namespace dlx
{
    Instruction::Instruction(const InstructionInfo& info) noexcept
        : m_Info(info)
    {}

    void Instruction::SetArgument(phi::u8 argument_number, InstructionArgument argument) noexcept
    {
        PHI_DBG_ASSERT(argument_number < 3u);

        switch (argument_number.get())
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
                PHI_DBG_ASSERT_NOT_REACHED();
#endif
        }
    }

    std::string Instruction::DebugInfo() const noexcept
    {
        switch (m_Info.GetNumberOfRequiredArguments().get())
        {
            case 0:
                return fmt::format("{}", magic_enum::enum_name(m_Info.GetOpCode()));
            case 1:
                return fmt::format("{}, {}", magic_enum::enum_name(m_Info.GetOpCode()),
                                   m_Arg1.DebugInfo());
            case 2:
                return fmt::format("{}, {}, {}", magic_enum::enum_name(m_Info.GetOpCode()),
                                   m_Arg1.DebugInfo(), m_Arg2.DebugInfo());
            case 3:
                return fmt::format("{}, {}, {}, {}", magic_enum::enum_name(m_Info.GetOpCode()),
                                   m_Arg1.DebugInfo(), m_Arg2.DebugInfo(), m_Arg3.DebugInfo());

#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_DBG_ASSERT_NOT_REACHED();
                break;
#endif
        }

#if !defined(DLXEMU_COVERAGE_BUILD)
        PHI_DBG_ASSERT_NOT_REACHED();
        return "Unknown";
#endif
    }

    void Instruction::Execute(Processor& processor) const noexcept
    {
        m_Info.Execute(processor, m_Arg1, m_Arg2, m_Arg3);
    }

    const InstructionInfo& Instruction::GetInfo() const noexcept
    {
        return m_Info;
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
