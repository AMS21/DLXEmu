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
        LTF,
        LTD,

        // Greater than
        SGT,
        SGTI,
        GTF,
        GTD,

        // Less than or equal
        SLE,
        SLEI,
        LEF,
        LED,

        // Greater than or equal
        SGE,
        SGEI,
        GEF,
        GED,

        // Equal
        SEQ,
        SEQI,
        EQF,
        EQD,

        // Not equal
        SNE,
        SNEI,
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

    OpCode StringToOpCode(std::string token);
} // namespace dlx
