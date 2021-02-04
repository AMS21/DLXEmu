#include "DLX/Instruction.hpp"

#include <Phi/Core/Assert.hpp>
#include "DLX/InstructionArg.hpp"

namespace dlx
{
    Instruction::Instruction(const InstructionInfo& info)
        : m_Info(info)
    {}

    void Instruction::SetArgument(phi::usize argument_number, InstructionArg argument)
    {
        PHI_ASSERT(argument_number < 3u);

        switch (argument_number.get())
        {
            case 0u:
                m_Arg1 = argument;
                break;
            case 1:
                m_Arg2 = argument;
                break;
            case 2:
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
} // namespace dlx
