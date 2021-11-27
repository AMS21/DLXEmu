#include "DLX/InstructionInfo.hpp"

namespace dlx
{
    void InstructionInfo::Execute(Processor& processor, const InstructionArgument& arg1,
                                  const InstructionArgument& arg2,
                                  const InstructionArgument& arg3) const noexcept
    {
        PHI_ASSERT(m_Executor, "No execution function defined");

        m_Executor(processor, arg1, arg2, arg3);
    }
} // namespace dlx
