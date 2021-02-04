#pragma once
#include <string>

namespace dlx
{
    enum class OpCode
    {
        // Not an actual instruction
        NONE = 0,

        /* Arithmetic */

        // Add
        ADD,
        ADDI,
        ADDU,
        ADDUI,

        // Sub
        SUB,
        SUBI,
        SUBU,
        SUBUI,

        // Multiplication
        MULT,
        MULTI,
        MULTU,
        MULTUI,

        // Division
        DIV,
        DIVI,
        DIVU,
        DIVUI,

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

        // Greater than
        SGT,
        SGTI,

        // Less than or equal
        SLE,
        SLEI,

        // Greater than or equal
        SGE,
        SGEI,

        // Equal
        SEQ,
        SEQI,

        // Not equal
        SNE,
        SNEI,

        /* Conditional branching */
        BEQZ,
        BNEZ,
        //BFPT,
        //BFPF,

        /* Unconditional branching */
        J,
        JR,
        JAL,
        JALR,

        /* Loading data */
        LB,
        LBU,
        LH,
        LHU,
        LW,
        LWU,
        //LF,
        //LD,

        /* Storing data */
        SB,
        SBU,
        SH,
        SHU,
        SW,
        SWU,
        //SF,
        //SD,

        /* Moving data */
        //MOVI2S,
        //MOVS2I,
        //MOVF,
        //MOVD,
        //MOVFP2I,
        //MOVI2FP,

        /* Floating pointer operations */
        //ADDF,
        //ADDD,
        //SUBF,
        //SUBD,
        //MULTF,
        //MULTD,
        //DIVF,
        //DIVD,

        /* Floating pointer conversions */
        //CVTF2D,
        //CVTF2I,
        //CVTD2F,
        //CVTD2I,
        //CVTI2F,
        //CVTI2D,

        /* Floating pointer comparison */
        //LTF,
        //LTD,
        //GTF,
        //GTD,
        //LEF,
        //LED,
        //GEF,
        //GED,
        //EQF,
        //EQD,
        //NEF,
        //NED,

        /* Other */
        TRAP,
        //RFE,
        HALT,
        NOP,

        // Special
        NUMBER_OF_ELEMENTS, // Always keep this element last
    };

    OpCode StringToOpCode(std::string token);
} // namespace dlx
