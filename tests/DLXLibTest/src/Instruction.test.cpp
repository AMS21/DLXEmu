#include <phi/test/test_macros.hpp>

#include <DLX/Instruction.hpp>
#include <DLX/InstructionArgument.hpp>
#include <DLX/InstructionInfo.hpp>
#include <DLX/InstructionLibrary.hpp>
#include <DLX/OpCode.hpp>
#include <DLX/RegisterNames.hpp>
#include <phi/core/types.hpp>

using namespace phi::literals;

TEST_CASE("Instruction")
{
    const dlx::InstructionInfo& add_info = dlx::LookUpIntructionInfo(dlx::OpCode::ADD);

    dlx::Instruction instr{add_info, 0u};

    CHECK(&instr.GetInfo() == &add_info);
    CHECK(bool(instr.GetSourceLine() == 0u));
    CHECK(instr.GetArg1().GetType() == dlx::ArgumentType::None);
    CHECK(instr.GetArg2().GetType() == dlx::ArgumentType::None);
    CHECK(instr.GetArg3().GetType() == dlx::ArgumentType::None);
    CHECK_FALSE(instr.DebugInfo().empty());

    dlx::InstructionArgument arg =
            dlx::ConstructInstructionArgumentRegisterInt(dlx::IntRegisterID::R1);
    instr.SetArgument(1_u8, arg);

    CHECK(instr.GetArg2().GetType() == dlx::ArgumentType::IntRegister);
}

TEST_CASE("Instruction DebugInfo")
{
    // 0 args
    dlx::Instruction nop{dlx::LookUpIntructionInfo(dlx::OpCode::NOP), 0u};
    CHECK_FALSE(nop.DebugInfo().empty());

    // 1 arg
    dlx::Instruction trap{dlx::LookUpIntructionInfo(dlx::OpCode::TRAP), 0u};
    CHECK_FALSE(trap.DebugInfo().empty());

    // 2 args
    dlx::Instruction lb{dlx::LookUpIntructionInfo(dlx::OpCode::LB), 0u};
    CHECK_FALSE(lb.DebugInfo().empty());

    // 3 args
    dlx::Instruction add{dlx::LookUpIntructionInfo(dlx::OpCode::ADD), 0u};
    CHECK_FALSE(add.DebugInfo().empty());
}
