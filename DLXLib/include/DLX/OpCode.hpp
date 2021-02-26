#pragma once
#include <string>

namespace dlx
{
    enum class OpCode
    {
        /* Arithmetic */

        // Add
        ADD,
        ADDI,
        ADDU,
        ADDUI,
        ADDF,
        ADDD,

        // Sub
        SUB,
        SUBI,
        SUBU,
        SUBUI,
        SUBF,
        SUBD,

        // Multiplication
        MULT,
        MULTI,
        MULTU,
        MULTUI,
        MULTF,
        MULTD,

        // Division
        DIV,
        DIVI,
        DIVU,
        DIVUI,
        DIVF,
        DIVD,

        // Shift left (logical)
        SLL,
        SLLI,

        // Shift right (logical)
        SRL,
        SRLI,

        // Shift left (arithmetic)
        SLA,
        SLAI,

        // Shift right (arithmetic)
        SRA,
        SRAI,

        /* Logic */

        // And
        AND,
        ANDI,

        // Or
        OR,
        ORI,

        // XOR
        XOR,
        XORI,

        /* Set conditionals */

        // Less than
        SLT,
        SLTI,
        SLTU,
        SLTUI,
        LTF,
        LTD,

        // Greater than
        SGT,
        SGTI,
        SGTU,
        SGTUI,
        GTF,
        GTD,

        // Less than or equal
        SLE,
        SLEI,
        SLEU,
        SLEUI,
        LEF,
        LED,

        // Greater than or equal
        SGE,
        SGEI,
        SGEU,
        SGEUI,
        GEF,
        GED,

        // Equal
        SEQ,
        SEQI,
        SEQU,
        SEQUI,
        EQF,
        EQD,

        // Not equal
        SNE,
        SNEI,
        SNEU,
        SNEUI,
        NEF,
        NED,

        /* Conditional branching */
        BEQZ,
        BNEZ,
        BFPT,
        BFPF,

        /* Unconditional branching */
        J,
        JR,
        JAL,
        JALR,

        /* Loading data */
        LHI,
        LB,
        LBU,
        LH,
        LHU,
        LW,
        LWU,
        LF,
        LD,

        /* Storing data */
        SB,
        SBU,
        SH,
        SHU,
        SW,
        SWU,
        SF,
        SD,

        /* Moving data */
        MOVF,
        MOVD,
        MOVFP2I,
        MOVI2FP,

        /* Converting data */
        CVTF2D,
        CVTF2I,
        CVTD2F,
        CVTD2I,
        CVTI2F,
        CVTI2D,

        /* Other */
        TRAP,
        HALT,
        NOP,

        // Special
        NUMBER_OF_ELEMENTS, // Always keep this element after all instructions

        // Not an actual instruction
        NONE,
    };

    OpCode StringToOpCode(std::string_view token);
} // namespace dlx
