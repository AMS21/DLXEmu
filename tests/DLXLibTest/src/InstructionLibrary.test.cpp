#include <catch2/catch_test_macros.hpp>

#include <DLX/InstructionInfo.hpp>
#include <DLX/InstructionLibrary.hpp>
#include <DLX/OpCode.hpp>
#include <phi/core/types.hpp>
#include <algorithm>
#include <array>

using namespace phi::literals;

TEST_CASE("InstructionLibrary")
{
    dlx::InstructionTableT table = dlx::GenerateInstructionTable();

    CHECK(std::is_sorted(table.begin(), table.end(),
                         [](dlx::InstructionInfo& lhs, dlx::InstructionInfo& rhs) {
                             return lhs.GetOpCode() < rhs.GetOpCode();
                         }));

    // Test all instruction infos
    for (int i{0}; i < static_cast<std::size_t>(dlx::OpCode::NUMBER_OF_ELEMENTS); ++i)
    {
        const dlx::OpCode opcode = static_cast<dlx::OpCode>(i);
        CHECK(table.at(i).GetOpCode() == opcode);

        const dlx::InstructionInfo& info = dlx::LookUpIntructionInfo(opcode);
        CHECK(info.GetOpCode() == opcode);
        CHECK(info.GetExecutor() != nullptr);
        CHECK((info.GetNumberOfRequiredArguments() >= 0).unsafe());
        CHECK(info.GetArgumentType(0_u8) != dlx::ArgumentType::Unknown);
        CHECK(info.GetArgumentType(1_u8) != dlx::ArgumentType::Unknown);
        CHECK(info.GetArgumentType(2_u8) != dlx::ArgumentType::Unknown);
    }
}
