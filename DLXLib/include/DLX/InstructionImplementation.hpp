#pragma once

#include "DLX/InstructionArg.hpp"
#include "DLX/InstructionInfo.hpp"
#include "InstructionLibrary.hpp"

namespace dlx::impl
{
    /* Arithmetic */

    // Add
    void ADD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Add immediate
    void ADDI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Add unsigned
    void ADDU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Add unsigned immediate
    void ADDUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3);

    // Add float
    void ADDF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Add double
    void ADDD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Subtract
    void SUB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Subtract immediate
    void SUBI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Subtract unsigned
    void SUBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Subtract unsigned immediate
    void SUBUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3);

    // Subtract float
    void SUBF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Subtract double
    void SUBD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Multiply
    void MULT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Multiply immediate
    void MULTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3);

    // Multiply unsigned
    void MULTU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3);

    // Multiply unsigned immediate
    void MULTUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3);

    // Multiply float
    void MULTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3);

    // Multiply double
    void MULTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3);

    // Divide
    void DIV(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Divide immediate
    void DIVI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Divide unsigned
    void DIVU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Divide unsigned immediate
    void DIVUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3);

    // Divide float
    void DIVF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Divide double
    void DIVD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Shift left logical
    void SLL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Shift left logical immediate
    void SLLI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Shift right logical
    void SRL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Shift right logical immediate
    void SRLI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Shift left arithmetic
    void SLA(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Shift left arithmetic immediate
    void SLAI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Shift right arithmetic
    void SRA(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Shift right arithmetic immediate
    void SRAI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    /* Logic */

    // And
    void AND(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // And immediate
    void ANDI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Or
    void OR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Or immediate
    void ORI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // XOR
    void XOR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // XOR immediate
    void XORI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    /* Condition testing */

    // Less than
    void SLT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Less than immediate
    void SLTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Less than float
    void LTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Less than double
    void LTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Greater than
    void SGT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Greater than immediate
    void SGTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Greater than float
    void GTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Greater than double
    void GTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Less than or equal
    void SLE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Less than or equal immediate
    void SLEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Less than or equal float
    void LEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Less than or equal double
    void LED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Greater than or equal
    void SGE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Greater than or equal immediate
    void SGEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Greater than or equal float
    void GEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Greater than or equal double
    void GED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Equal
    void SEQ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Equal immediate
    void SEQI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Equal float
    void EQF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Equal double
    void EQD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Not equal
    void SNE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Not equal immediate
    void SNEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Not equal float
    void NEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Not equal double
    void NED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    /* Conditional branching */

    // Branch equal zero
    void BEQZ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Branch not equal zero
    void BNEZ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Branch floating point true
    void BFPT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Branch floating point false
    void BFPF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    /* Unconditional Branching */

    // Jump
    void J(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
           const InstructionArg& arg3);

    // Jump register
    void JR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Jump and Link
    void JAL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Jump and link register
    void JALR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    /* Loading data */

    // Load high immediate
    void LHI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Load byte
    void LB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Load unsigned byte
    void LBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Load half word (2 bytes)
    void LH(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Load unsigned half word (2 bytes)
    void LHU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Load word (4 bytes)
    void LW(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Load unsigned word (4 bytes)
    void LWU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Load float (4 bytes)
    void LF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Load double (8 bytes)
    void LD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    /* Storing data */

    // Store byte
    void SB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Store unsigned byte
    void SBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Store half word (2 bytes)
    void SH(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Store unsigned half word (2 bytes)
    void SHU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Store word (4 bytes)
    void SW(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Store unsigned word (4 bytes)
    void SWU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);

    // Store float (4 bytes)
    void SF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    // Store double (8 bytes)
    void SD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3);

    /* Moving data */

    // Move float
    void MOVF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Move double
    void MOVD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Move float to int
    void MOVFP2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3);

    // Move int to float
    void MOVI2FP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3);

    /* Converting data */

    // Convert float to double
    void CVTF2D(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3);

    // Convert float to int
    void CVTF2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3);

    // Convert double to float
    void CVTD2F(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3);

    // Convert double to int
    void CVTD2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3);

    // Convert int to float
    void CVTI2F(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3);

    // Convert int to double
    void CVTI2D(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3);

    /* Special */

    // Trap
    void TRAP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // Halt
    void HALT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3);

    // NOPeration
    void NOP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3);
} // namespace dlx::impl
