#include "DLX/Instruction.hpp"

#include "DLX/InstructionArg.hpp"
#include <Phi/Core/Assert.hpp>

namespace dlx
{
    Instruction::Instruction(const InstructionInfo& info)
        : m_Info(info)
    {}

    void Instruction::SetArgument(phi::u8 argument_number, InstructionArg argument)
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
        return std::string("Instruction");
    }

    void Instruction::Execute(Processor& processor) const noexcept
    {
        m_Info.Execute(processor, m_Arg1, m_Arg2, m_Arg3);
    }

    const InstructionInfo& Instruction::GetInfo() const
    {
        return m_Info;
    }

    const InstructionArg& Instruction::GetArg1() const
    {
        return m_Arg1;
    }

    const InstructionArg& Instruction::GetArg2() const
    {
        return m_Arg2;
    }

    const InstructionArg& Instruction::GetArg3() const
    {
        return m_Arg3;
    }
} // namespace dlx
