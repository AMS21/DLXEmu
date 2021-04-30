#pragma once

#include "DLX/InstructionArg.hpp"
#include "DLX/InstructionInfo.hpp"
#include "InstructionLibrary.hpp"

namespace dlx::impl
{
    /* Arithmetic */

    // Add
    void ADD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Add immediate
    void ADDI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Add unsigned
    void ADDU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Add unsigned immediate
    void ADDUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Add float
    void ADDF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Add double
    void ADDD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Subtract
    void SUB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Subtract immediate
    void SUBI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Subtract unsigned
    void SUBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Subtract unsigned immediate
    void SUBUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Subtract float
    void SUBF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Subtract double
    void SUBD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Multiply
    void MULT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Multiply immediate
    void MULTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Multiply unsigned
    void MULTU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Multiply unsigned immediate
    void MULTUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3) noexcept;

    // Multiply float
    void MULTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Multiply double
    void MULTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Divide
    void DIV(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Divide immediate
    void DIVI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Divide unsigned
    void DIVU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Divide unsigned immediate
    void DIVUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Divide float
    void DIVF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Divide double
    void DIVD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Shift left logical
    void SLL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Shift left logical immediate
    void SLLI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Shift right logical
    void SRL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Shift right logical immediate
    void SRLI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Shift left arithmetic
    void SLA(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Shift left arithmetic immediate
    void SLAI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Shift right arithmetic
    void SRA(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Shift right arithmetic immediate
    void SRAI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    /* Logic */

    // And
    void AND(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // And immediate
    void ANDI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Or
    void OR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Or immediate
    void ORI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // XOR
    void XOR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // XOR immediate
    void XORI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    /* Condition testing */

    // Less than
    void SLT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Less than immediate
    void SLTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Less than unsigned
    void SLTU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Less than unsigned iimmediate
    void SLTUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Less than float
    void LTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Less than double
    void LTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Greater than
    void SGT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Greater than immediate
    void SGTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Greater than unsigned
    void SGTU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Greater than unsigned immediate
    void SGTUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Greater than float
    void GTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Greater than double
    void GTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Less than or equal
    void SLE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Less than or equal immediate
    void SLEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Less than or equal unsigned
    void SLEU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Less than or equal unsigned immediate
    void SLEUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Less than or equal float
    void LEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Less than or equal double
    void LED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Greater than or equal
    void SGE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Greater than or equal immediate
    void SGEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Greater than or equal unsigned
    void SGEU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Greater than or equal unsigned immediate
    void SGEUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Greater than or equal float
    void GEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Greater than or equal double
    void GED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Equal
    void SEQ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Equal immediate
    void SEQI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Equal unsigned
    void SEQU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Equal unsigned immediate
    void SEQUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    // Equal float
    void EQF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Equal double
    void EQD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Not equal
    void SNE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Not equal immediate
    void SNEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Not equal float
    void NEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Not equal double
    void NED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Not equal unsigned
    void SNEU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // No equal unsigned immediate
    void SNEUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3) noexcept;

    /* Conditional branching */

    // Branch equal zero
    void BEQZ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Branch not equal zero
    void BNEZ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Branch floating point true
    void BFPT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Branch floating point false
    void BFPF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    /* Unconditional Branching */

    // Jump
    void J(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
           const InstructionArg& arg3) noexcept;

    // Jump register
    void JR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Jump and Link
    void JAL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Jump and link register
    void JALR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    /* Loading data */

    // Load high immediate
    void LHI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Load byte
    void LB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Load unsigned byte
    void LBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Load half word (2 bytes)
    void LH(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Load unsigned half word (2 bytes)
    void LHU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Load word (4 bytes)
    void LW(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Load unsigned word (4 bytes)
    void LWU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Load float (4 bytes)
    void LF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Load double (8 bytes)
    void LD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    /* Storing data */

    // Store byte
    void SB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Store unsigned byte
    void SBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Store half word (2 bytes)
    void SH(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Store unsigned half word (2 bytes)
    void SHU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Store word (4 bytes)
    void SW(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Store unsigned word (4 bytes)
    void SWU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;

    // Store float (4 bytes)
    void SF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    // Store double (8 bytes)
    void SD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
            const InstructionArg& arg3) noexcept;

    /* Moving data */

    // Move float
    void MOVF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Move double
    void MOVD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Move float to int
    void MOVFP2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3) noexcept;

    // Move int to float
    void MOVI2FP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3) noexcept;

    /* Converting data */

    // Convert float to double
    void CVTF2D(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3) noexcept;

    // Convert float to int
    void CVTF2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3) noexcept;

    // Convert double to float
    void CVTD2F(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3) noexcept;

    // Convert double to int
    void CVTD2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3) noexcept;

    // Convert int to float
    void CVTI2F(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3) noexcept;

    // Convert int to double
    void CVTI2D(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3) noexcept;

    /* Special */

    // Trap
    void TRAP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // Halt
    void HALT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
              const InstructionArg& arg3) noexcept;

    // NOPeration
    void NOP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
             const InstructionArg& arg3) noexcept;
} // namespace dlx::impl
