#include "DLX/InstructionLibrary.hpp"

#include "DLX/InstructionArgument.hpp"
#include "DLX/InstructionImplementation.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/OpCode.hpp"
#include <phi/compiler_support/warning.hpp>
#include <algorithm>
#include <array>

namespace dlx
{
    constexpr void InitInstruction(InstructionTableT& table, OpCode opcode, ArgumentType arg1,
                                   ArgumentType arg2, ArgumentType arg3,
                                   RegisterAccessType  register_access_type,
                                   InstructionExecutor executor) noexcept
    {
        PHI_ASSERT(table.at(static_cast<phi::size_t>(opcode)).GetExecutor() == nullptr,
                   "Already registered instruction {}", dlx::enum_name(opcode));

        table.at(static_cast<phi::size_t>(opcode)) =
                InstructionInfo(opcode, arg1, arg2, arg3, register_access_type, executor);
    }

    constexpr InstructionTableT InitializeInstructionTable() noexcept
    {
        InstructionTableT table;

        /* Arithmetic */

        // Addition
        InitInstruction(table, OpCode::ADD, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::ADD);
        InitInstruction(table, OpCode::ADDI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::ADDI);
        InitInstruction(table, OpCode::ADDU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::ADDU);
        InitInstruction(table, OpCode::ADDUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::ADDUI);
        InitInstruction(table, OpCode::ADDF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Float, impl::ADDF);
        InitInstruction(table, OpCode::ADDD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Double, impl::ADDD);

        // Subtraction
        InitInstruction(table, OpCode::SUB, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SUB);
        InitInstruction(table, OpCode::SUBI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SUBI);
        InitInstruction(table, OpCode::SUBU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SUBU);
        InitInstruction(table, OpCode::SUBUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SUBUI);
        InitInstruction(table, OpCode::SUBF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Float, impl::SUBF);
        InitInstruction(table, OpCode::SUBD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Double, impl::SUBD);

        // Multiplication
        InitInstruction(table, OpCode::MULT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::MULT);
        InitInstruction(table, OpCode::MULTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::MULTI);
        InitInstruction(table, OpCode::MULTU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::MULTU);
        InitInstruction(table, OpCode::MULTUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::MULTUI);
        InitInstruction(table, OpCode::MULTF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Float, impl::MULTF);
        InitInstruction(table, OpCode::MULTD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Double, impl::MULTD);

        // Division
        InitInstruction(table, OpCode::DIV, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::DIV);
        InitInstruction(table, OpCode::DIVI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::DIVI);
        InitInstruction(table, OpCode::DIVU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::DIVU);
        InitInstruction(table, OpCode::DIVUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::DIVUI);
        InitInstruction(table, OpCode::DIVF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Float, impl::DIVF);
        InitInstruction(table, OpCode::DIVD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        RegisterAccessType::Double, impl::DIVD);

        // Shift left logical
        InitInstruction(table, OpCode::SLL, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SLL);
        InitInstruction(table, OpCode::SLLI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SLLI);

        // Shift right logical
        InitInstruction(table, OpCode::SRL, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SRL);
        InitInstruction(table, OpCode::SRLI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SRLI);

        // Shift left arithmetic
        InitInstruction(table, OpCode::SLA, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SLA);
        InitInstruction(table, OpCode::SLAI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SLAI);

        // Shift right arithmetic
        InitInstruction(table, OpCode::SRA, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SRA);
        InitInstruction(table, OpCode::SRAI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SRAI);

        /* Logical */

        // And
        InitInstruction(table, OpCode::AND, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::AND);
        InitInstruction(table, OpCode::ANDI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::ANDI);

        // Or
        InitInstruction(table, OpCode::OR, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::OR);
        InitInstruction(table, OpCode::ORI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::ORI);

        // Xor
        InitInstruction(table, OpCode::XOR, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::XOR);
        InitInstruction(table, OpCode::XORI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::XORI);

        /* Conditionals */

        // Set less than
        InitInstruction(table, OpCode::SLT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SLT);
        InitInstruction(table, OpCode::SLTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SLTI);
        InitInstruction(table, OpCode::SLTU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SLTU);
        InitInstruction(table, OpCode::SLTUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SLTUI);
        InitInstruction(table, OpCode::LTF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::LTF);
        InitInstruction(table, OpCode::LTD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Double,
                        impl::LTD);

        // Set greater than
        InitInstruction(table, OpCode::SGT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SGT);
        InitInstruction(table, OpCode::SGTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SGTI);
        InitInstruction(table, OpCode::SGTU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SGTU);
        InitInstruction(table, OpCode::SGTUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SGTUI);
        InitInstruction(table, OpCode::GTF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::GTF);
        InitInstruction(table, OpCode::GTD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Double,
                        impl::GTD);

        // Set less than or equal
        InitInstruction(table, OpCode::SLE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SLE);
        InitInstruction(table, OpCode::SLEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SLEI);
        InitInstruction(table, OpCode::SLEU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SLEU);
        InitInstruction(table, OpCode::SLEUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SLEUI);
        InitInstruction(table, OpCode::LEF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::LEF);
        InitInstruction(table, OpCode::LED, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Double,
                        impl::LED);

        // Set greater than or equal
        InitInstruction(table, OpCode::SGE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SGE);
        InitInstruction(table, OpCode::SGEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SGEI);
        InitInstruction(table, OpCode::SGEU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SGEU);
        InitInstruction(table, OpCode::SGEUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SGEUI);
        InitInstruction(table, OpCode::GEF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::GEF);
        InitInstruction(table, OpCode::GED, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Double,
                        impl::GED);

        // Set equal
        InitInstruction(table, OpCode::SEQ, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SEQ);
        InitInstruction(table, OpCode::SEQI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SEQI);
        InitInstruction(table, OpCode::SEQU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SEQU);
        InitInstruction(table, OpCode::SEQUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SEQUI);
        InitInstruction(table, OpCode::EQF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::EQF);
        InitInstruction(table, OpCode::EQD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Double,
                        impl::EQD);

        // Set not equal
        InitInstruction(table, OpCode::SNE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SNE);
        InitInstruction(table, OpCode::SNEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SNEI);
        InitInstruction(table, OpCode::SNEU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SNEU);
        InitInstruction(table, OpCode::SNEUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SNEUI);
        InitInstruction(table, OpCode::NEF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::NEF);
        InitInstruction(table, OpCode::NED, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Double,
                        impl::NED);

        /* Conditional branching */

        // Branch equal zero
        InitInstruction(table, OpCode::BEQZ, ArgumentType::IntRegister, ArgumentType::Label,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BEQZ);

        // Branch not equal zero
        InitInstruction(table, OpCode::BNEZ, ArgumentType::IntRegister, ArgumentType::Label,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BNEZ);

        // Branch floating point true
        InitInstruction(table, OpCode::BFPT, ArgumentType::Label, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BFPT);

        // Branch floating point false
        InitInstruction(table, OpCode::BFPF, ArgumentType::Label, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BFPF);

        /* Unconditional branching */

        // Jump
        InitInstruction(table, OpCode::J, ArgumentType::Label, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::None, impl::J);

        // Jump to register
        InitInstruction(table, OpCode::JR, ArgumentType::IntRegister, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::JR);

        // Jump and link
        InitInstruction(table, OpCode::JAL, ArgumentType::Label, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::JAL);

        // Jump and link to register
        InitInstruction(table, OpCode::JALR, ArgumentType::IntRegister, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::JALR);

        /* Loading data */

        // Load high immediate
        InitInstruction(table, OpCode::LHI, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, ArgumentType::None,
                        RegisterAccessType::Signed, impl::LHI);

        // Load byte
        InitInstruction(table, OpCode::LB, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Signed, impl::LB);

        // Load byte unsigned
        InitInstruction(table, OpCode::LBU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::MixedSignedUnsigned, impl::LBU);

        // Load half word
        InitInstruction(table, OpCode::LH, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Signed, impl::LH);

        // Load half word unsigned
        InitInstruction(table, OpCode::LHU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::MixedSignedUnsigned, impl::LHU);

        // Load word
        InitInstruction(table, OpCode::LW, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Signed, impl::LW);

        // Load word unsigned
        InitInstruction(table, OpCode::LWU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::MixedSignedUnsigned, impl::LWU);

        // Load float
        InitInstruction(table, OpCode::LF, ArgumentType::FloatRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::MixedFloatSigned, impl::LF);

        // Load double
        InitInstruction(table, OpCode::LD, ArgumentType::FloatRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::MixedDoubleSigned, impl::LD);

        /* Storing data */

        // Store byte
        InitInstruction(table, OpCode::SB,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Signed,
                        impl::SB);

        // Store byte unsigned
        InitInstruction(table, OpCode::SBU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None,
                        RegisterAccessType::MixedSignedUnsigned, impl::SBU);

        // Store half word
        InitInstruction(table, OpCode::SH,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Signed,
                        impl::SH);

        // Store half word unsigned
        InitInstruction(table, OpCode::SHU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None,
                        RegisterAccessType::MixedSignedUnsigned, impl::SHU);

        // Store word
        InitInstruction(table, OpCode::SW,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Signed,
                        impl::SW);

        // Store word unsigned
        InitInstruction(table, OpCode::SWU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None,
                        RegisterAccessType::MixedSignedUnsigned, impl::SWU);

        // Store float
        InitInstruction(table, OpCode::SF,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::MixedFloatSigned, impl::SF);

        // Store double
        InitInstruction(table, OpCode::SD,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::MixedDoubleSigned, impl::SD);

        /* Moving data */

        // Move float
        InitInstruction(table, OpCode::MOVF, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::MOVF);

        // Move double
        InitInstruction(table, OpCode::MOVD, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Double,
                        impl::MOVD);

        // Move float to int
        InitInstruction(table, OpCode::MOVFP2I, ArgumentType::IntRegister,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::MixedFloatSigned, impl::MOVFP2I);

        // Move int to float
        InitInstruction(table, OpCode::MOVI2FP, ArgumentType::FloatRegister,
                        ArgumentType::IntRegister, ArgumentType::None,
                        RegisterAccessType::MixedFloatSigned, impl::MOVI2FP);

        /* Converting data */

        // Convert float to double
        InitInstruction(table, OpCode::CVTF2D, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::MixedFloatDouble, impl::CVTF2D);

        // Convert float to int
        InitInstruction(table, OpCode::CVTF2I, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::CVTF2I);

        // Convert double to float
        InitInstruction(table, OpCode::CVTD2F, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::MixedFloatDouble, impl::CVTD2F);

        // Convert double to int
        InitInstruction(table, OpCode::CVTD2I, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::MixedFloatDouble, impl::CVTD2I);

        // Convert int to float
        InitInstruction(table, OpCode::CVTI2F, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::CVTI2F);

        // Convert int to double
        InitInstruction(table, OpCode::CVTI2D, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::MixedFloatDouble, impl::CVTI2D);

        /* Special */

        // Trap
        InitInstruction(table, OpCode::TRAP, ArgumentType::ImmediateInteger, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::None, impl::TRAP);

        // Halt
        InitInstruction(table, OpCode::HALT, ArgumentType::None, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::None, impl::HALT);

        // No operation
        InitInstruction(table, OpCode::NOP, ArgumentType::None, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::None, impl::NOP);

        return table;
    }

    InstructionTableT GenerateInstructionTable() noexcept
    {
        return InitializeInstructionTable();
    }

    constexpr const static InstructionTableT instruction_table = InitializeInstructionTable();

    const InstructionInfo& LookUpInstructionInfo(OpCode instruction) noexcept
    {
        return instruction_table.at(static_cast<phi::size_t>(instruction));
    }
} // namespace dlx
