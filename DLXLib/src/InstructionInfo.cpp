#include "DLX/InstructionInfo.hpp"

#include "DLX/InstructionArgument.hpp"
#include <phi/core/assert.hpp>

namespace dlx
{
    void InstructionInfo::Execute(Processor& processor, const InstructionArgument& arg1,
                                  const InstructionArgument& arg2,
                                  const InstructionArgument& arg3) const noexcept
    {
        PHI_ASSERT(m_Executor, "No execution function defined");

        // Make sure non arguments are marked as unknown
        PHI_ASSERT(arg1.GetType() != ArgumentType::Unknown, "Arg1 type is unknown");
        PHI_ASSERT(arg2.GetType() != ArgumentType::Unknown, "Arg2 type is unknown");
        PHI_ASSERT(arg3.GetType() != ArgumentType::Unknown, "Arg3 type is unknown");

        // Make sure argument types match
        PHI_ASSERT(ArgumentTypeIncludes(arg1.GetType(), m_Arg1Type),
                   "Unexpected argument type for arg1");
        PHI_ASSERT(ArgumentTypeIncludes(arg2.GetType(), m_Arg2Type),
                   "Unexpected argument type for arg2");
        PHI_ASSERT(ArgumentTypeIncludes(arg3.GetType(), m_Arg3Type),
                   "Unexpected argument type for arg3");

        // Execute the instruction using the specified executor
        m_Executor(processor, arg1, arg2, arg3);
    }
} // namespace dlx
