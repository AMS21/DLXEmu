#pragma once

#include "DLX/InstructionInfo.hpp"
#include <array>

namespace dlx
{
    class Processor;
    class InstructionArg;

    // Class holding references to the InstructionInfos about each and every instruction
    class InstructionLibrary
    {
    public:
        InstructionLibrary();

        phi::Boolean Initialize();

        const InstructionInfo& LookUp(OpCode instruction) const noexcept;

    private:
        void InitInstruction(OpCode opcode, ArgumentType arg1, ArgumentType arg2, ArgumentType arg3,
                             InstructionExecutor executor);

    private:
        std::array<InstructionInfo, static_cast<std::size_t>(OpCode::NUMBER_OF_ELEMENTS)>
                m_Instructions;
    };
} // namespace dlx
