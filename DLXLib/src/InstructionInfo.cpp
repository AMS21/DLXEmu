#include "DLX/InstructionInfo.hpp"

namespace dlx
{
    void InstructionInfo::Execute(Processor& processor, const InstructionArg& arg1,
                                  const InstructionArg& arg2,
                                  const InstructionArg& arg3) const noexcept
    {
        PHI_ASSERT(m_Executor, "No execution function defined");

        m_Executor(processor, arg1, arg2, arg3);
    }
} // namespace dlx
