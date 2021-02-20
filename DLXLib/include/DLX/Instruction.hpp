#pragma once

#include "DLX/InstructionArg.hpp"
#include "DLX/InstructionLibrary.hpp"

// Class represents a single fully parsed dlx assembler instruction
namespace dlx
{
    class Instruction
    {
    public:
        explicit Instruction(const InstructionInfo& info);

        void SetArgument(phi::u8 argument_number, InstructionArg argument);

        std::string DebugInfo() const noexcept;

        void Execute(Processor& processor) const noexcept;

        [[nodiscard]] const InstructionInfo& GetInfo() const;

        [[nodiscard]] const InstructionArg& GetArg1() const;

        [[nodiscard]] const InstructionArg& GetArg2() const;

        [[nodiscard]] const InstructionArg& GetArg3() const;

    private:
        const InstructionInfo& m_Info;

        InstructionArg m_Arg1;
        InstructionArg m_Arg2;
        InstructionArg m_Arg3;
    };
} // namespace dlx
