#include "DLX/InstructionLibrary.hpp"

#include "DLX/InstructionArg.hpp"
#include "DLX/InstructionImplementation.hpp"
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
        // Arithmetic
        InitInstruction(OpCode::ADD, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::ADD);
        InitInstruction(OpCode::ADDI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::ADDI);
        InitInstruction(OpCode::ADDU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::ADDU);
        InitInstruction(OpCode::ADDUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::ADDUI);
        InitInstruction(OpCode::SUB, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SUB);
        InitInstruction(OpCode::SUBI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SUBI);
        InitInstruction(OpCode::SUBU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SUBU);
        InitInstruction(OpCode::SUBUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SUBUI);
        InitInstruction(OpCode::MULT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::MULT);
        InitInstruction(OpCode::MULTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::MULTI);
        InitInstruction(OpCode::MULTU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::MULTU);
        InitInstruction(OpCode::MULTUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::MULTUI);
        InitInstruction(OpCode::DIV, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::DIV);
        InitInstruction(OpCode::DIVI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::DIVI);
        InitInstruction(OpCode::DIVU, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::DIVU);
        InitInstruction(OpCode::DIVUI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::DIVUI);
        InitInstruction(OpCode::SLL, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SLL);
        InitInstruction(OpCode::SLLI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SLLI);
        InitInstruction(OpCode::SRL, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SRL);
        InitInstruction(OpCode::SRLI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SRLI);
        InitInstruction(OpCode::SLA, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SLA);
        InitInstruction(OpCode::SLAI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SLAI);
        InitInstruction(OpCode::SRA, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SRA);
        InitInstruction(OpCode::SRAI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SRAI);
        // Logic
        InitInstruction(OpCode::AND, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::AND);
        InitInstruction(OpCode::ANDI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::ANDI);
        InitInstruction(OpCode::OR, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::OR);
        InitInstruction(OpCode::ORI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::ORI);
        InitInstruction(OpCode::XOR, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::XOR);
        InitInstruction(OpCode::XORI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::XORI);
        // Conditionals
        InitInstruction(OpCode::SLT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SLT);
        InitInstruction(OpCode::SLTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SLTI);
        InitInstruction(OpCode::SGT, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SGT);
        InitInstruction(OpCode::SGTI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SGTI);
        InitInstruction(OpCode::SLE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SLE);
        InitInstruction(OpCode::SLEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SLEI);
        InitInstruction(OpCode::SGE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SGE);
        InitInstruction(OpCode::SGEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SGEI);
        InitInstruction(OpCode::SEQ, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SEQ);
        InitInstruction(OpCode::SEQI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SEQI);
        InitInstruction(OpCode::SNE, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::IntRegister, impl::SNE);
        InitInstruction(OpCode::SNEI, ArgumentType::IntRegister, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger, impl::SNEI);
        // Conditional branching
        InitInstruction(OpCode::BEQZ, ArgumentType::IntRegister, ArgumentType::Label,
                        ArgumentType::None, impl::BEQZ);
        InitInstruction(OpCode::BNEZ, ArgumentType::IntRegister, ArgumentType::Label,
                        ArgumentType::None, impl::BNEZ);
        // Unconditional branching
        InitInstruction(OpCode::J, ArgumentType::Label, ArgumentType::None, ArgumentType::None,
                        impl::J);
        InitInstruction(OpCode::JR, ArgumentType::IntRegister, ArgumentType::None,
                        ArgumentType::None, impl::JR);
        InitInstruction(OpCode::JAL, ArgumentType::Label, ArgumentType::None, ArgumentType::None,
                        impl::JAL);
        InitInstruction(OpCode::JALR, ArgumentType::IntRegister, ArgumentType::None,
                        ArgumentType::None, impl::JALR);
        // Loading data
        InitInstruction(OpCode::LB, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, impl::LB);
        InitInstruction(OpCode::LBU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, impl::LBU);
        InitInstruction(OpCode::LH, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, impl::LH);
        InitInstruction(OpCode::LHU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, impl::LHU);
        InitInstruction(OpCode::LW, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, impl::LW);
        InitInstruction(OpCode::LWU, ArgumentType::IntRegister,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::None, impl::LWU);
        // Storing data
        InitInstruction(OpCode::SB,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, impl::SB);
        InitInstruction(OpCode::SBU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, impl::SBU);
        InitInstruction(OpCode::SH,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, impl::SH);
        InitInstruction(OpCode::SHU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, impl::SHU);
        InitInstruction(OpCode::SW,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, impl::SW);
        InitInstruction(OpCode::SWU,
                        ArgumentType::ImmediateInteger | ArgumentType::AddressDisplacement,
                        ArgumentType::IntRegister, ArgumentType::None, impl::SWU);
        // Special
        InitInstruction(OpCode::TRAP, ArgumentType::None, ArgumentType::None, ArgumentType::None,
                        impl::TRAP);
        InitInstruction(OpCode::HALT, ArgumentType::None, ArgumentType::None, ArgumentType::None,
                        impl::HALT);
        InitInstruction(OpCode::NOP, ArgumentType::None, ArgumentType::None, ArgumentType::None,
                        impl::NOP);

        return true;
    }

    const InstructionInfo& InstructionLibrary::LookUp(OpCode instruction) const noexcept
    {
        return m_Instructions.at(static_cast<std::size_t>(instruction));
    }

    void InstructionLibrary::InitInstruction(OpCode opcode, ArgumentType arg1, ArgumentType arg2,
                                             ArgumentType arg3, InstructionExecutor executor)
    {
        PHI_ASSERT(LookUp(opcode).GetExecutor() == nullptr, "Already registered instruction {}",
                   magic_enum::enum_name(opcode));

        m_Instructions.at(static_cast<std::size_t>(opcode)) =
                InstructionInfo(opcode, arg1, arg2, arg3, executor);
    }
} // namespace dlx
