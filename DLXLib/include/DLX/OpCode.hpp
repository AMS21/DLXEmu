#pragma once

#include "DLX/EnumName.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace dlx
{
#define DLX_ENUM_OPCODE                                                                            \
    /* Arithmetic */                                                                               \
                                                                                                   \
    /* Add */                                                                                      \
    DLX_ENUM_OPCODE_IMPL(ADD)                                                                      \
    DLX_ENUM_OPCODE_IMPL(ADDI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(ADDU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(ADDUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(ADDF)                                                                     \
    DLX_ENUM_OPCODE_IMPL(ADDD)                                                                     \
                                                                                                   \
    /* Sub */                                                                                      \
    DLX_ENUM_OPCODE_IMPL(SUB)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SUBI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SUBU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SUBUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(SUBF)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SUBD)                                                                     \
                                                                                                   \
    /* Multiplication */                                                                           \
    DLX_ENUM_OPCODE_IMPL(MULT)                                                                     \
    DLX_ENUM_OPCODE_IMPL(MULTI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(MULTU)                                                                    \
    DLX_ENUM_OPCODE_IMPL(MULTUI)                                                                   \
    DLX_ENUM_OPCODE_IMPL(MULTF)                                                                    \
    DLX_ENUM_OPCODE_IMPL(MULTD)                                                                    \
                                                                                                   \
    /* Division */                                                                                 \
    DLX_ENUM_OPCODE_IMPL(DIV)                                                                      \
    DLX_ENUM_OPCODE_IMPL(DIVI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(DIVU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(DIVUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(DIVF)                                                                     \
    DLX_ENUM_OPCODE_IMPL(DIVD)                                                                     \
                                                                                                   \
    /* Shift left (logical) */                                                                     \
    DLX_ENUM_OPCODE_IMPL(SLL)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SLLI)                                                                     \
                                                                                                   \
    /* Shift right (logical) */                                                                    \
    DLX_ENUM_OPCODE_IMPL(SRL)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SRLI)                                                                     \
                                                                                                   \
    /* Shift left (arithmetic) */                                                                  \
    DLX_ENUM_OPCODE_IMPL(SLA)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SLAI)                                                                     \
                                                                                                   \
    /* Shift right (arithmetic) */                                                                 \
    DLX_ENUM_OPCODE_IMPL(SRA)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SRAI)                                                                     \
                                                                                                   \
    /* Logic */                                                                                    \
                                                                                                   \
    /* And */                                                                                      \
    DLX_ENUM_OPCODE_IMPL(AND)                                                                      \
    DLX_ENUM_OPCODE_IMPL(ANDI)                                                                     \
                                                                                                   \
    /* Or */                                                                                       \
    DLX_ENUM_OPCODE_IMPL(OR)                                                                       \
    DLX_ENUM_OPCODE_IMPL(ORI)                                                                      \
                                                                                                   \
    /* XOR */                                                                                      \
    DLX_ENUM_OPCODE_IMPL(XOR)                                                                      \
    DLX_ENUM_OPCODE_IMPL(XORI)                                                                     \
                                                                                                   \
    /* Set conditionals */                                                                         \
                                                                                                   \
    /* Less than */                                                                                \
    DLX_ENUM_OPCODE_IMPL(SLT)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SLTI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SLTU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SLTUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(LTF)                                                                      \
    DLX_ENUM_OPCODE_IMPL(LTD)                                                                      \
                                                                                                   \
    /* Greater than */                                                                             \
    DLX_ENUM_OPCODE_IMPL(SGT)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SGTI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SGTU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SGTUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(GTF)                                                                      \
    DLX_ENUM_OPCODE_IMPL(GTD)                                                                      \
                                                                                                   \
    /* Less than or equal */                                                                       \
    DLX_ENUM_OPCODE_IMPL(SLE)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SLEI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SLEU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SLEUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(LEF)                                                                      \
    DLX_ENUM_OPCODE_IMPL(LED)                                                                      \
                                                                                                   \
    /* Greater than or equal */                                                                    \
    DLX_ENUM_OPCODE_IMPL(SGE)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SGEI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SGEU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SGEUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(GEF)                                                                      \
    DLX_ENUM_OPCODE_IMPL(GED)                                                                      \
                                                                                                   \
    /* Equal */                                                                                    \
    DLX_ENUM_OPCODE_IMPL(SEQ)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SEQI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SEQU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SEQUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(EQF)                                                                      \
    DLX_ENUM_OPCODE_IMPL(EQD)                                                                      \
                                                                                                   \
    /* Not equal */                                                                                \
    DLX_ENUM_OPCODE_IMPL(SNE)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SNEI)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SNEU)                                                                     \
    DLX_ENUM_OPCODE_IMPL(SNEUI)                                                                    \
    DLX_ENUM_OPCODE_IMPL(NEF)                                                                      \
    DLX_ENUM_OPCODE_IMPL(NED)                                                                      \
                                                                                                   \
    /* Conditional branching */                                                                    \
    DLX_ENUM_OPCODE_IMPL(BEQZ)                                                                     \
    DLX_ENUM_OPCODE_IMPL(BNEZ)                                                                     \
    DLX_ENUM_OPCODE_IMPL(BFPT)                                                                     \
    DLX_ENUM_OPCODE_IMPL(BFPF)                                                                     \
                                                                                                   \
    /* Unconditional branching */                                                                  \
    DLX_ENUM_OPCODE_IMPL(J)                                                                        \
    DLX_ENUM_OPCODE_IMPL(JR)                                                                       \
    DLX_ENUM_OPCODE_IMPL(JAL)                                                                      \
    DLX_ENUM_OPCODE_IMPL(JALR)                                                                     \
                                                                                                   \
    /* Loading data */                                                                             \
    DLX_ENUM_OPCODE_IMPL(LHI)                                                                      \
    DLX_ENUM_OPCODE_IMPL(LB)                                                                       \
    DLX_ENUM_OPCODE_IMPL(LBU)                                                                      \
    DLX_ENUM_OPCODE_IMPL(LH)                                                                       \
    DLX_ENUM_OPCODE_IMPL(LHU)                                                                      \
    DLX_ENUM_OPCODE_IMPL(LW)                                                                       \
    DLX_ENUM_OPCODE_IMPL(LWU)                                                                      \
    DLX_ENUM_OPCODE_IMPL(LF)                                                                       \
    DLX_ENUM_OPCODE_IMPL(LD)                                                                       \
                                                                                                   \
    /* Storing data */                                                                             \
    DLX_ENUM_OPCODE_IMPL(SB)                                                                       \
    DLX_ENUM_OPCODE_IMPL(SBU)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SH)                                                                       \
    DLX_ENUM_OPCODE_IMPL(SHU)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SW)                                                                       \
    DLX_ENUM_OPCODE_IMPL(SWU)                                                                      \
    DLX_ENUM_OPCODE_IMPL(SF)                                                                       \
    DLX_ENUM_OPCODE_IMPL(SD)                                                                       \
                                                                                                   \
    /* Moving data */                                                                              \
    DLX_ENUM_OPCODE_IMPL(MOVD)                                                                     \
    DLX_ENUM_OPCODE_IMPL(MOVF)                                                                     \
    DLX_ENUM_OPCODE_IMPL(MOVFP2I)                                                                  \
    DLX_ENUM_OPCODE_IMPL(MOVI2FP)                                                                  \
                                                                                                   \
    /* Converting data */                                                                          \
    DLX_ENUM_OPCODE_IMPL(CVTF2D)                                                                   \
    DLX_ENUM_OPCODE_IMPL(CVTF2I)                                                                   \
    DLX_ENUM_OPCODE_IMPL(CVTD2F)                                                                   \
    DLX_ENUM_OPCODE_IMPL(CVTD2I)                                                                   \
    DLX_ENUM_OPCODE_IMPL(CVTI2F)                                                                   \
    DLX_ENUM_OPCODE_IMPL(CVTI2D)                                                                   \
                                                                                                   \
    /* Other */                                                                                    \
    DLX_ENUM_OPCODE_IMPL(TRAP)                                                                     \
    DLX_ENUM_OPCODE_IMPL(HALT)                                                                     \
    DLX_ENUM_OPCODE_IMPL(NOP)

    enum class OpCode
    {
#define DLX_ENUM_OPCODE_IMPL(name) name,
        DLX_ENUM_OPCODE
#undef DLX_ENUM_OPCODE_IMPL

                // Special
                NUMBER_OF_ELEMENTS, // Always keep this element after all instructions

        // Not an actual instruction
        NONE,
    };

    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(
            5264) // C5264: 'dlx::NumberOfOpCodes': 'const' variable is not used

    static constexpr const phi::usize NumberOfOpCodes{
            static_cast<phi::size_t>(OpCode::NUMBER_OF_ELEMENTS)};

    PHI_MSVC_SUPPRESS_WARNING_POP()

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wreturn-type")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702)

    template <>
    [[nodiscard]] constexpr phi::string_view enum_name<OpCode>(OpCode value) noexcept
    {
        switch (value)
        {
#define DLX_ENUM_OPCODE_IMPL(name)                                                                 \
    case OpCode::name:                                                                             \
        return #name;

            DLX_ENUM_OPCODE

#undef DLX_ENUM_OPCODE_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

    [[nodiscard]] OpCode StringToOpCode(phi::string_view token) noexcept;
} // namespace dlx
