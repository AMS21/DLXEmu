#pragma once

#include "DLX/InstructionInfo.hpp"
#include "DLX/InstructionArg.hpp"

// Class represents a single fully parsed dlx assembler instruction
namespace dlx
{
    class Instruction
    {
    public:
        explicit Instruction(const InstructionInfo& info) noexcept;

        void SetArgument(phi::u8 argument_number, InstructionArg argument) noexcept;

        [[nodiscard]] std::string DebugInfo() const noexcept;

        void Execute(Processor& processor) const noexcept;

        [[nodiscard]] const InstructionInfo& GetInfo() const noexcept;

        [[nodiscard]] const InstructionArg& GetArg1() const noexcept;

        [[nodiscard]] const InstructionArg& GetArg2() const noexcept;

        [[nodiscard]] const InstructionArg& GetArg3() const noexcept;

    private:
        const InstructionInfo& m_Info;

        InstructionArg m_Arg1;
        InstructionArg m_Arg2;
        InstructionArg m_Arg3;
    };
} // namespace dlx
