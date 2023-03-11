#pragma once

#include "DLX/InstructionArgument.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/OpCode.hpp"
#include <phi/compiler_support/warning.hpp>

// Class represents a single fully parsed dlx assembler instruction
namespace dlx
{
    class Instruction
    {
    public:
        explicit Instruction(const InstructionInfo& info, const phi::u64 source_line) noexcept;

        void SetArgument(phi::u8 argument_number, InstructionArgument argument) noexcept;

        PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

        [[nodiscard]] std::string DebugInfo() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_POP()

        void Execute(Processor& processor) const noexcept;

        [[nodiscard]] const InstructionInfo& GetInfo() const noexcept;

        [[nodiscard]] const phi::u64 GetSourceLine() const noexcept;

        [[nodiscard]] const InstructionArgument& GetArg1() const noexcept;

        [[nodiscard]] const InstructionArgument& GetArg2() const noexcept;

        [[nodiscard]] const InstructionArgument& GetArg3() const noexcept;

    private:
        const InstructionInfo& m_Info;

        phi::u64 m_SourceLine;

        InstructionArgument m_Arg1;
        InstructionArgument m_Arg2;
        InstructionArgument m_Arg3;
    };
} // namespace dlx
