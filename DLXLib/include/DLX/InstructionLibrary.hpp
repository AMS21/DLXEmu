#pragma once

#include "DLX/InstructionInfo.hpp"
#include <array>

namespace dlx
{
    // Class holding references to the InstructionInfos about each and every instruction
    class InstructionLibrary
    {
    public:
        InstructionLibrary() noexcept;

        phi::Boolean Initialize() noexcept;

        const InstructionInfo& LookUp(OpCode instruction) const noexcept;

    protected:
        void InitInstruction(OpCode opcode, ArgumentType arg1, ArgumentType arg2, ArgumentType arg3,
                             RegisterAccessType  register_access_type,
                             InstructionExecutor executor) noexcept;

    private:
        std::array<InstructionInfo, static_cast<std::size_t>(OpCode::NUMBER_OF_ELEMENTS)>
                m_Instructions;
    };
} // namespace dlx
