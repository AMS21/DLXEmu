#include "DLX/Instruction.hpp"

#include "DLX/InstructionArg.hpp"
#include <Phi/Core/Assert.hpp>
#include <magic_enum.hpp>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/fmt.h>

namespace dlx
{
    Instruction::Instruction(const InstructionInfo& info) noexcept
        : m_Info(info)
    {}

    void Instruction::SetArgument(phi::u8 argument_number, InstructionArg argument) noexcept
    {
        PHI_ASSERT(argument_number < 3u);

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
            default:
                PHI_ASSERT_NOT_REACHED();
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
            default:
                PHI_ASSERT_NOT_REACHED();
                break;
        }

        PHI_ASSERT_NOT_REACHED();
        return "Unknown";
    }

    void Instruction::Execute(Processor& processor) const noexcept
    {
        m_Info.Execute(processor, m_Arg1, m_Arg2, m_Arg3);
    }

    const InstructionInfo& Instruction::GetInfo() const noexcept
    {
        return m_Info;
    }

    const InstructionArg& Instruction::GetArg1() const noexcept
    {
        return m_Arg1;
    }

    const InstructionArg& Instruction::GetArg2() const noexcept
    {
        return m_Arg2;
    }

    const InstructionArg& Instruction::GetArg3() const noexcept
    {
        return m_Arg3;
    }
} // namespace dlx
