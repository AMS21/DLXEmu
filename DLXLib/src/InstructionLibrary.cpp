#include "DLX/InstructionLibrary.hpp"

#include "DLX/InstructionArg.hpp"
#include "DLX/InstructionImplementation.hpp"
#include "DLX/InstructionInfo.hpp"
#include <magic_enum.hpp>

namespace dlx
{
    InstructionLibrary::InstructionLibrary()
        : m_Instructions()
    {
        PHI_ASSERT(Initialize());
    }

    phi::Boolean InstructionLibrary::Initialize()
    {
        /* Arithmetic */

        // Addition
        InitInstruction(OpCode::ADD, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::ADD);
        InitInstruction(OpCode::ADDI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::ADDI);
        InitInstruction(OpCode::ADDU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::ADDU);
        InitInstruction(OpCode::ADDUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::ADDUI);
        InitInstruction(OpCode::ADDF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Float, impl::ADDF);
        InitInstruction(OpCode::ADDD, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Double, impl::ADDD);

        // Subtraction
        InitInstruction(OpCode::SUB, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::SUB);
        InitInstruction(OpCode::SUBI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::SUBI);
        InitInstruction(OpCode::SUBU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::SUBU);
        InitInstruction(OpCode::SUBUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::SUBUI);
        InitInstruction(OpCode::SUBF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Float, impl::SUBF);
        InitInstruction(OpCode::SUBD, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Double, impl::SUBD);

        // Multiplikation
        InitInstruction(OpCode::MULT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::MULT);
        InitInstruction(OpCode::MULTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::MULTI);
        InitInstruction(OpCode::MULTU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::MULTU);
        InitInstruction(OpCode::MULTUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::MULTUI);
        InitInstruction(OpCode::MULTF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Float, impl::MULTF);
        InitInstruction(OpCode::MULTD, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Double, impl::MULTD);

        // Division
        InitInstruction(OpCode::DIV, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Signed, impl::DIV);
        InitInstruction(OpCode::DIVI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Signed, impl::DIVI);
        InitInstruction(OpCode::DIVU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Unsigned, impl::DIVU);
        InitInstruction(OpCode::DIVUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Unsigned, impl::DIVUI);
        InitInstruction(OpCode::DIVF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Float, impl::DIVF);
        InitInstruction(OpCode::DIVD, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::Double, impl::DIVD);

        // Shift left logical
        InitInstruction(OpCode::SLL, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SLL);
        InitInstruction(OpCode::SLLI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SLLI);

        // Shift right logical
        InitInstruction(OpCode::SRL, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SRL);
        InitInstruction(OpCode::SRLI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SRLI);

        // Shift left arithmetic
        InitInstruction(OpCode::SLA, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SLA);
        InitInstruction(OpCode::SLAI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SLAI);

        // Shift right arithmetic
        InitInstruction(OpCode::SRA, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SRA);
        InitInstruction(OpCode::SRAI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SRAI);

        /* Logical */

        // And
        InitInstruction(OpCode::AND, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::AND);
        InitInstruction(OpCode::ANDI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::ANDI);

        // Or
        InitInstruction(OpCode::OR, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::OR);
        InitInstruction(OpCode::ORI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::ORI);

        // Xor
        InitInstruction(OpCode::XOR, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::XOR);
        InitInstruction(OpCode::XORI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::XORI);

        /* Conditionals */

        // Set less than
        InitInstruction(OpCode::SLT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SLT);
        InitInstruction(OpCode::SLTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SLTI);
        InitInstruction(OpCode::LTF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::LTF);
        InitInstruction(OpCode::LTD, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::LTD);

        // Set greater than
        InitInstruction(OpCode::SGT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SGT);
        InitInstruction(OpCode::SGTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SGTI);
        InitInstruction(OpCode::GTF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::GTF);
        InitInstruction(OpCode::GTD, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::GTD);

        // Set less than or equal
        InitInstruction(OpCode::SLE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SLE);
        InitInstruction(OpCode::SLEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SLEI);
        InitInstruction(OpCode::LEF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::LEF);
        InitInstruction(OpCode::LED, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::LED);

        // Set greater than or equal
        InitInstruction(OpCode::SGE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SGE);
        InitInstruction(OpCode::SGEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SGEI);
        InitInstruction(OpCode::GEF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::GEF);
        InitInstruction(OpCode::GED, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::GED);

        // Set equal
        InitInstruction(OpCode::SEQ, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SEQ);
        InitInstruction(OpCode::SEQI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SEQI);
        InitInstruction(OpCode::EQF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::EQF);
        InitInstruction(OpCode::EQD, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::EQD);

        // Set not equal
        InitInstruction(OpCode::SNE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, RegisterAccessType::Ignored, impl::SNE);
        InitInstruction(OpCode::SNEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, RegisterAccessType::Ignored, impl::SNEI);
        InitInstruction(OpCode::NEF, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::NEF);
        InitInstruction(OpCode::NED, ArgumentType::FloatRegister, ArgumentType::FloatRegister,
                        ArgumentType::FloatRegister, RegisterAccessType::MixedFloatDouble,
                        impl::NED);

        /* Conditional branching */

        // Branch equal zero
        InitInstruction(OpCode::BEQZ, ArgumentType::IntRegister, ArgumentType::Label,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BEQZ);

        // Branch not equal zero
        InitInstruction(OpCode::BNEZ, ArgumentType::IntRegister, ArgumentType::Label,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BNEZ);

        // Branch floating point true
        InitInstruction(OpCode::BFPT, ArgumentType::FloatRegister, ArgumentType::Label,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BFPT);

        // Branch floating point false
        InitInstruction(OpCode::BFPF, ArgumentType::FloatRegister, ArgumentType::Label,
                        ArgumentType::None, RegisterAccessType::Ignored, impl::BFPF);

        /* Unconditional branching */

        // Jump
        InitInstruction(OpCode::J, ArgumentType::Label, ArgumentType::None, ArgumentType::None,
                        RegisterAccessType::None, impl::J);

        // Jump to register
        InitInstruction(OpCode::JR, ArgumentType::IntRegister, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::JR);

        // Jump and link
        InitInstruction(OpCode::JAL, ArgumentType::Label, ArgumentType::None, ArgumentType::None,
                        RegisterAccessType::Unsigned, impl::JAL);

        // Jump and link to register
        InitInstruction(OpCode::JALR, ArgumentType::IntRegister, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::JALR);

        /* Loading data */

        // Load byte
        InitInstruction(OpCode::LB, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Signed, impl::LB);

        // Load byte unsigned
        InitInstruction(OpCode::LBU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::LBU);

        // Load half word
        InitInstruction(OpCode::LH, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Signed, impl::LH);

        // Load half word unsigned
        InitInstruction(OpCode::LHU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::LHU);

        // Load word
        InitInstruction(OpCode::LW, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Signed, impl::LW);

        // Load word unsigned
        InitInstruction(OpCode::LWU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Unsigned, impl::LWU);

        // Load float
        InitInstruction(OpCode::LF, ArgumentType::FloatRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Float, impl::LF);

        // Load double
        InitInstruction(OpCode::LD, ArgumentType::FloatRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, RegisterAccessType::Double, impl::LD);

        /* Storing data */

        // Store byte
        InitInstruction(OpCode::SB,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Signed,
                        impl::SB);

        // Store byte unsigned
        InitInstruction(OpCode::SBU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Unsigned,
                        impl::SBU);

        // Store half word
        InitInstruction(OpCode::SH,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Signed,
                        impl::SH);

        // Store half word unsigned
        InitInstruction(OpCode::SHU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Unsigned,
                        impl::SHU);

        // Store word
        InitInstruction(OpCode::SW,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Signed,
                        impl::SW);

        // Store word unsigned
        InitInstruction(OpCode::SWU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, RegisterAccessType::Unsigned,
                        impl::SWU);

        // Store float
        InitInstruction(OpCode::SF,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::FloatRegister, ArgumentType::None, RegisterAccessType::Float,
                        impl::SF);

        // Store double
        InitInstruction(OpCode::SD,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::FloatRegister, ArgumentType::None,
                        RegisterAccessType::Double, impl::SD);

        /* Special */

        // Trap
        InitInstruction(OpCode::TRAP, ArgumentType::ImmediateInteger, ArgumentType::None,
                        ArgumentType::None, RegisterAccessType::None, impl::TRAP);

        // Halt
        InitInstruction(OpCode::HALT, ArgumentType::None, ArgumentType::None, ArgumentType::None,
                        RegisterAccessType::None, impl::HALT);

        // No operation
        InitInstruction(OpCode::NOP, ArgumentType::None, ArgumentType::None, ArgumentType::None,
                        RegisterAccessType::None, impl::NOP);

        return true;
    }

    const InstructionInfo& InstructionLibrary::LookUp(OpCode instruction) const noexcept
    {
        return m_Instructions.at(static_cast<std::size_t>(instruction));
    }

    void InstructionLibrary::InitInstruction(OpCode opcode, ArgumentType arg1, ArgumentType arg2,
                                             ArgumentType        arg3,
                                             RegisterAccessType  register_access_type,
                                             InstructionExecutor executor)
    {
        PHI_ASSERT(LookUp(opcode).GetExecutor() == nullptr, "Already registered instruction {}",
                   magic_enum::enum_name(opcode));

        m_Instructions.at(static_cast<std::size_t>(opcode)) =
                InstructionInfo(opcode, arg1, arg2, arg3, register_access_type, executor);
    }
} // namespace dlx
