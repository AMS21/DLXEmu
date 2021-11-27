#pragma once

#include "DLX/InstructionArgument.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/OpCode.hpp"

// Class represents a single fully parsed dlx assembler instruction
namespace dlx
{
    class Instruction
    {
    public:
        explicit Instruction(const InstructionInfo& info) noexcept;

        void SetArgument(phi::u8 argument_number, InstructionArgument argument) noexcept;

        [[nodiscard]] std::string DebugInfo() const noexcept;

        void Execute(Processor& processor) const noexcept;

        [[nodiscard]] const InstructionInfo& GetInfo() const noexcept;

        [[nodiscard]] const InstructionArgument& GetArg1() const noexcept;

        [[nodiscard]] const InstructionArgument& GetArg2() const noexcept;

        [[nodiscard]] const InstructionArgument& GetArg3() const noexcept;

    private:
        const InstructionInfo& m_Info;

        InstructionArgument m_Arg1;
        InstructionArgument m_Arg2;
        InstructionArgument m_Arg3;
    };
} // namespace dlx
