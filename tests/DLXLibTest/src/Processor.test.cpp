#include <phi/test/test_macros.hpp>

#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <DLX/RegisterNames.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/types.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wfloat-equal")
PHI_CLANG_SUPPRESS_WARNING("-Wexit-time-destructors")
PHI_CLANG_SUPPRESS_WARNING("-Wglobal-constructors")
PHI_GCC_SUPPRESS_WARNING("-Wuseless-cast")

static dlx::Processor     proc;
static dlx::ParsedProgram res;

constexpr std::int32_t  signed_min   = phi::i32::limits_type::min();
constexpr std::int32_t  signed_max   = phi::i32::limits_type::max();
constexpr std::uint32_t unsigned_max = phi::u32::limits_type::max();

// Correct Implementation
TEST_CASE("ADD")
{
    res = dlx::Parser::Parse("ADD R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 6);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 6);
}

TEST_CASE("ADDI")
{
    res = dlx::Parser::Parse("ADDI R1 R2 #30");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 32);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
}

TEST_CASE("ADDU")
{
    res = dlx::Parser::Parse("ADDU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 19u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 19u);
}

TEST_CASE("ADDUI")
{
    res = dlx::Parser::Parse("ADDUI R1 R2 #19");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
}

TEST_CASE("ADDF")
{
    res = dlx::Parser::Parse("ADDF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f + 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).unsafe() == 2.0f);
}

TEST_CASE("ADDD")
{
    res = dlx::Parser::Parse("ADDD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 1.0 + 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);
}

TEST_CASE("SUB")
{
    res = dlx::Parser::Parse("SUB R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 50);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 30);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 20);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 50);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 30);
}

TEST_CASE("SUBI")
{
    res = dlx::Parser::Parse("SUBI R1 R2 #25");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 50);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 25);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 50);
}

TEST_CASE("SUBU")
{
    res = dlx::Parser::Parse("SUBU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 50u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 30u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 20u);
}

TEST_CASE("SUBUI")
{
    res = dlx::Parser::Parse("SUBUI R1 R2 #25");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 50u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 25u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 50u);
}

TEST_CASE("SUBF")
{
    res = dlx::Parser::Parse("SUBF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 2.0f - 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).unsafe() == 1.0f);
}

TEST_CASE("SUBD")
{
    res = dlx::Parser::Parse("SUBD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 2.0 - 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);
}

TEST_CASE("MULT")
{
    res = dlx::Parser::Parse("MULT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("MULTI")
{
    res = dlx::Parser::Parse("MULTI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
}

TEST_CASE("MULTU")
{
    res = dlx::Parser::Parse("MULTU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 6u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);
}

TEST_CASE("MULTUI")
{
    res = dlx::Parser::Parse("MULTUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 6u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
}

TEST_CASE("MULTF")
{
    res = dlx::Parser::Parse("MULTF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 3.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 3.0f * 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 3.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).unsafe() == 2.0f);
}

TEST_CASE("MULTD")
{
    res = dlx::Parser::Parse("MULTD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 3.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 3.0 * 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 3.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);
}

TEST_CASE("DIV")
{
    res = dlx::Parser::Parse("DIV R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 6);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 2);

    // Divide by zero
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("DIVI")
{
    res = dlx::Parser::Parse("DIVI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 6);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 6);

    // Divide by zero
    res = dlx::Parser::Parse("DIVI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 6);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("DIVU")
{
    res = dlx::Parser::Parse("DIVU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 6u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 6u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 2u);

    // Divide by zero
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("DIVUI")
{
    res = dlx::Parser::Parse("DIVUI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 6u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 6u);

    // Divide by zero

    res = dlx::Parser::Parse("DIVUI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();
    CHECK(proc.IsHalted());
}

TEST_CASE("DIVF")
{
    res = dlx::Parser::Parse("DIVF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 12.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 12.0f / 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 12.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).unsafe() == 2.0f);
}

TEST_CASE("DIVD")
{
    res = dlx::Parser::Parse("DIVD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 12.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 12.0 / 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 12.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);
}

TEST_CASE("SLL")
{
    res = dlx::Parser::Parse("SLL R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 16);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
}

TEST_CASE("SLLI")
{
    res = dlx::Parser::Parse("SLLI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 32);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);

    res = dlx::Parser::Parse("SLLI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
}

TEST_CASE("SRL")
{
    res = dlx::Parser::Parse("SRL R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 4);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 2147483647);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == -1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
}

TEST_CASE("SRLI")
{
    res = dlx::Parser::Parse("SRLI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1073741823);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -1);

    res = dlx::Parser::Parse("SRLI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
}

TEST_CASE("SLA")
{
    res = dlx::Parser::Parse("SLA R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 16);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
}

TEST_CASE("SLAI")
{
    res = dlx::Parser::Parse("SLAI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 32);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);

    res = dlx::Parser::Parse("SLAI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
}

TEST_CASE("SRA")
{
    res = dlx::Parser::Parse("SRA R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   -2147483647); // 0b10000000'00000000'00000000'00000001
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    // 0b11000000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == -1073741824);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -2147483647);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   1073741825); // 0b01000000'00000000'00000000'00000001
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    // 0b00100000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 536870912);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1073741825);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
}

TEST_CASE("SRAI")
{
    res = dlx::Parser::Parse("SRAI R1 R2 #1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   -2147483647); // 0b10000000'00000000'00000000'00000001

    proc.ExecuteCurrentProgram();

    // 0b11000000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == -1073741824);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -2147483647);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   1073741825); // 0b01000000'00000000'00000000'00000001

    proc.ExecuteCurrentProgram();

    // 0b00100000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 536870912);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1073741825);

    res = dlx::Parser::Parse("SRAI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    // 0b11000000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 8);
}

TEST_CASE("AND")
{
    res = dlx::Parser::Parse("AND R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("ANDI")
{
    res = dlx::Parser::Parse("ANDI R1 R2 #5");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
}

TEST_CASE("OR")
{
    res = dlx::Parser::Parse("OR R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("ORI")
{
    res = dlx::Parser::Parse("ORI R1 R2 #8");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 9);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
}

TEST_CASE("XOR")
{
    res = dlx::Parser::Parse("XOR R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 7);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 7);
}

TEST_CASE("XORI")
{
    res = dlx::Parser::Parse("XORI R1 R2 #7");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
}

TEST_CASE("SLT")
{
    res = dlx::Parser::Parse("SLT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 5);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("SLTI")
{
    res = dlx::Parser::Parse("SLTI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 4);
}

TEST_CASE("SLTU")
{
    res = dlx::Parser::Parse("SLTU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 5u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 5u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);
}

TEST_CASE("SLTUI")
{
    res = dlx::Parser::Parse("SLTUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 4u);
}

TEST_CASE("LTF")
{
    res = dlx::Parser::Parse("LTF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 2.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);
}

TEST_CASE("LTD")
{
    res = dlx::Parser::Parse("LTD F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);
}

TEST_CASE("SGT")
{
    res = dlx::Parser::Parse("SGT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("SGTI")
{
    res = dlx::Parser::Parse("SGTI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 4);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
}

TEST_CASE("SGTU")
{
    res = dlx::Parser::Parse("SGTU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);
}

TEST_CASE("SGTUI")
{
    res = dlx::Parser::Parse("SGTUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 4u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
}

TEST_CASE("GTF")
{
    res = dlx::Parser::Parse("GTF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 2.0f);
}
TEST_CASE("GTD")
{
    res = dlx::Parser::Parse("GTD F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);
}
TEST_CASE("SLE")
{
    res = dlx::Parser::Parse("SLE R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("SLEI")
{
    res = dlx::Parser::Parse("SLEI R1 R2 #3");

    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 4);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
}

TEST_CASE("SLEU")
{
    res = dlx::Parser::Parse("SLEU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 5u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);
}

TEST_CASE("SLEUI")
{
    res = dlx::Parser::Parse("SLEUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 4u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
}

TEST_CASE("LEF")
{
    res = dlx::Parser::Parse("LEF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 2.0f);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);
}

TEST_CASE("LED")
{
    res = dlx::Parser::Parse("LED F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);
}

TEST_CASE("SGE")
{
    res = dlx::Parser::Parse("SGE R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 5);
}

TEST_CASE("SGEI")
{
    res = dlx::Parser::Parse("SGEI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 4);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
}

TEST_CASE("SGEU")
{
    res = dlx::Parser::Parse("SGEU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 5u);
}

TEST_CASE("SGEUI")
{
    res = dlx::Parser::Parse("SGEUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 4u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
}

TEST_CASE("GEF")
{
    res = dlx::Parser::Parse("GEF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 2.0f);
}

TEST_CASE("GED")
{
    res = dlx::Parser::Parse("GED F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);
}

TEST_CASE("SEQ")
{
    res = dlx::Parser::Parse("SEQ R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("SEQI")
{
    res = dlx::Parser::Parse("SEQI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
}

TEST_CASE("SEQU")
{
    res = dlx::Parser::Parse("SEQU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);
}

TEST_CASE("SEQUI")
{
    res = dlx::Parser::Parse("SEQUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
}

TEST_CASE("EQF")
{
    res = dlx::Parser::Parse("EQF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 2.0f);
}

TEST_CASE("EQD")
{
    res = dlx::Parser::Parse("EQD F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 2.0);
}

TEST_CASE("SNE")
{
    res = dlx::Parser::Parse("SNE R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
}

TEST_CASE("SNEI")
{
    res = dlx::Parser::Parse("SNEI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 3);
}

TEST_CASE("SNEU")
{
    res = dlx::Parser::Parse("SNEU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3u);
}

TEST_CASE("SNEUI")
{
    res = dlx::Parser::Parse("SNEUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 3u);
}

TEST_CASE("NEF")
{
    res = dlx::Parser::Parse("NEF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);
}

TEST_CASE("NED")
{
    res = dlx::Parser::Parse("NED F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).unsafe() == 1.0);
}

TEST_CASE("BEQZ")
{
    const char* data =
            R"(
            BEQZ R1 true
            HALT
        true:
            ADDI R2 R0 #1
        )";
    res = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 0);
}

TEST_CASE("BNEZ")
{
    const char* data =
            R"(
            BNEZ R1 true
            HALT
        true:
            ADDI R2 R0 #1
        )";
    res = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 0);
}

TEST_CASE("BFPT")
{
    const char* data =
            R"(
            BFPT true
            HALT
        true:
            ADDI R1 R0 #1
        )";
    res = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(true);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(false);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
}

TEST_CASE("BFPF")
{
    const char* data =
            R"(
            BFPF false
            HALT
        false:
            ADDI R1 R0 #1
        )";
    res = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(false);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(true);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
}

TEST_CASE("J")
{
    const char* data = R"(
            J jump_label
            HALT

        jump_label:
            ADDI R1 R0 #1
        )";
    res              = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
}

TEST_CASE("JR")
{
    const char* data = R"(
            JR R1
            HALT

            ADDI R2 R0 #1
        )";

    res = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
}

TEST_CASE("JAL")
{
    const char* data = R"(
            JAL jump_label
            HALT

        jump_label:
            ADDI R1 R0 #1
        )";
    res              = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R31, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R31).unsafe() == 1);
}

TEST_CASE("JALR")
{
    const char* data = R"(
            JALR R1
            HALT

            ADDI R2 R0 #1
        )";

    res = dlx::Parser::Parse(data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R31).unsafe() == 1);
}

TEST_CASE("LHI")
{
    res = dlx::Parser::Parse("LHI R1 #1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == (1 << 16));
}

TEST_CASE("LB")
{
    res = dlx::Parser::Parse("LB R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreByte(1000u, static_cast<std::int8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 21);

    res = dlx::Parser::Parse("LB R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreByte(1000u, static_cast<std::int8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 21);
}

TEST_CASE("LBU")
{
    res = dlx::Parser::Parse("LBU R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedByte(1000u, static_cast<std::uint8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21u);

    res = dlx::Parser::Parse("LBU R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedByte(1000u, static_cast<std::uint8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21);
}

TEST_CASE("LH")
{
    res = dlx::Parser::Parse("LH R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreHalfWord(1000u, static_cast<std::int16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 21);

    res = dlx::Parser::Parse("LH R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreHalfWord(1000u, static_cast<std::int16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 21);
}

TEST_CASE("LHU")
{
    res = dlx::Parser::Parse("LHU R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedHalfWord(1000u, static_cast<std::uint16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21u);

    res = dlx::Parser::Parse("LHU R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedHalfWord(1000u, static_cast<std::uint16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21);
}

TEST_CASE("LW")
{
    res = dlx::Parser::Parse("LW R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreWord(1000u, 21);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 21);

    res = dlx::Parser::Parse("LW R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreWord(1000u, 21);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 21);
}

TEST_CASE("LWU")
{
    res = dlx::Parser::Parse("LWU R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedWord(1000u, 21u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21u);

    res = dlx::Parser::Parse("LWU R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedWord(1000u, 21u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 21);
}

TEST_CASE("LF")
{
    res = dlx::Parser::Parse("LF F0 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

    proc.ClearMemory();
    proc.GetMemory().StoreFloat(1000u, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() == 1.0f);

    res = dlx::Parser::Parse("LF F0 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

    proc.ClearMemory();
    proc.GetMemory().StoreFloat(1000u, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() == 1.0f);
}

TEST_CASE("LD")
{
    res = dlx::Parser::Parse("LD F0 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);

    proc.ClearMemory();
    proc.GetMemory().StoreDouble(1000u, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 1.0);

    res = dlx::Parser::Parse("LD F0 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);

    proc.ClearMemory();
    proc.GetMemory().StoreDouble(1000u, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 1.0);
}

TEST_CASE("SB")
{
    res = dlx::Parser::Parse("SB #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21);

    res = dlx::Parser::Parse("SB 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21);
}

TEST_CASE("SBU")
{
    res = dlx::Parser::Parse("SBU #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadUnsignedByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21u);

    res = dlx::Parser::Parse("SBU 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadUnsignedByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21u);
}

TEST_CASE("SH")
{
    res = dlx::Parser::Parse("SH #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21);

    res = dlx::Parser::Parse("SH 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21);
}

TEST_CASE("SHU")
{
    res = dlx::Parser::Parse("SHU #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadUnsignedHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21u);

    res = dlx::Parser::Parse("SHU 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadUnsignedHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21u);
}

TEST_CASE("SW")
{
    res = dlx::Parser::Parse("SW #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21);

    res = dlx::Parser::Parse("SW 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21);
}

TEST_CASE("SWU")
{
    res = dlx::Parser::Parse("SWU #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadUnsignedWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21u);

    res = dlx::Parser::Parse("SWU 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadUnsignedWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 21u);
}

TEST_CASE("SF")
{
    res = dlx::Parser::Parse("SF #1000 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadFloat(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 1.0f);

    res = dlx::Parser::Parse("SF 1000(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadFloat(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 1.0f);
}

TEST_CASE("SD")
{
    res = dlx::Parser::Parse("SD #1000 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, 1.0);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadDouble(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 1.0);

    res = dlx::Parser::Parse("SD 1000(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, 1.0);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadDouble(1000u);
    REQUIRE(val.has_value());
    CHECK(val->unsafe() == 1.0);
}

TEST_CASE("MOVF")
{
    res = dlx::Parser::Parse("MOVF F0 F1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).unsafe() == 1.0f);
}

TEST_CASE("MOVD")
{
    res = dlx::Parser::Parse("MOVD F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
}

TEST_CASE("MOVFP2I")
{
    res = dlx::Parser::Parse("MOVFP2I R1 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() == 1.0f);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() != -1);
}

TEST_CASE("MOVI2FP")
{
    res = dlx::Parser::Parse("MOVI2FP F0 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 1);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() != -1.0f);
}

TEST_CASE("CVTF2D")
{
    res = dlx::Parser::Parse("CVTF2D F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).unsafe() == 1.0);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);
}

TEST_CASE("CVTF2I")
{
    res = dlx::Parser::Parse("CVTF2I F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() != -1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);
}

TEST_CASE("CVTD2F")
{
    res = dlx::Parser::Parse("CVTD2F F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() == 1.0f);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
}

TEST_CASE("CVTD2I")
{
    res = dlx::Parser::Parse("CVTD2I F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() != -1.0f);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
}

TEST_CASE("CVTI2F")
{
    res = dlx::Parser::Parse("CVTI2F F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() != -1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).unsafe() == 1.0f);
}

TEST_CASE("CVTI2D")
{
    res = dlx::Parser::Parse("CVTI2D F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).unsafe() != 1.0f);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).unsafe() == 1.0);
}

TEST_CASE("TRAP")
{
    res = dlx::Parser::Parse("TRAP #1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("HALT")
{
    res = dlx::Parser::Parse("HALT");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("NOP")
{
    res = dlx::Parser::Parse("NOP");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

// Processor - Operation exceptions
TEST_CASE("Signed addition overflow")
{
    res = dlx::Parser::Parse("ADD R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_min + 4);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 5);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
}

TEST_CASE("Signed addition underflow")
{
    res = dlx::Parser::Parse("ADD R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == -1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max - 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == -2);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
}

TEST_CASE("Unsigned addition overflow")
{
    res = dlx::Parser::Parse("ADDU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, unsigned_max);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == unsigned_max);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == unsigned_max);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 0u);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, unsigned_max);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 4u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == unsigned_max);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 5u);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
}

TEST_CASE("Signed subtraction overflow")
{
    res = dlx::Parser::Parse("SUB R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_min + 4);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == -5);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
}

TEST_CASE("Signed subtraction underflow")
{
    res = dlx::Parser::Parse("SUB R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 0);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max - 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 2);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
}

TEST_CASE("Unsigned subtraction underflow")
{
    res = dlx::Parser::Parse("SUBU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 0u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 0u);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 0u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 1u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == unsigned_max);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 1u);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 0u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == unsigned_max - 4u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 5u);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
}

TEST_CASE("Signed multiplication overflow")
{
    res = dlx::Parser::Parse("MULT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == -2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 2);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, signed_max);

    proc.ExecuteCurrentProgram();

    //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == signed_max);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
}

TEST_CASE("Signed multiplication underflow")
{
    res = dlx::Parser::Parse("MULT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, signed_min);

    proc.ExecuteCurrentProgram();

    //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == signed_min);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == signed_max);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == signed_min);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
}

TEST_CASE("Unsigned multiplication overflow")
{
    res = dlx::Parser::Parse("MULTU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, unsigned_max);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    //CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).unsafe() == unsigned_max);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).unsafe() == 3);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
}

TEST_CASE("Signed division by zero")
{
    res = dlx::Parser::Parse("DIV R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();
    CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
    CHECK(proc.IsHalted());

    res = dlx::Parser::Parse("DIVI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);

    proc.ExecuteCurrentProgram();
    CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
    CHECK(proc.IsHalted());
}

TEST_CASE("Unsigned division by zero")
{
    res = dlx::Parser::Parse("DIVU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

    proc.ExecuteCurrentProgram();
    CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
    CHECK(proc.IsHalted());

    res = dlx::Parser::Parse("DIVUI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);

    proc.ExecuteCurrentProgram();
    CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
    CHECK(proc.IsHalted());
}

TEST_CASE("Float division by zero")
{
    res = dlx::Parser::Parse("DIVF F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F4, 0.0f);

    proc.ExecuteCurrentProgram();
    CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
    CHECK(proc.IsHalted());
}

TEST_CASE("Double division by zero")
{
    res = dlx::Parser::Parse("DIVD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 0.0f);

    proc.ExecuteCurrentProgram();
    CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
    CHECK(proc.IsHalted());
}

TEST_CASE("Shift left bad shift")
{
    // Logical
    res = dlx::Parser::Parse("SLL R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 32);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 999999);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == -1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

    // Arithmetic
    res = dlx::Parser::Parse("SLA R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 32);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 999999);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == -1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);
}

TEST_CASE("Shift right bad shift")
{
    // Logical
    res = dlx::Parser::Parse("SRL R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 32);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 999999);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == -1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

    // Arithmetic
    res = dlx::Parser::Parse("SRA R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == -1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 32);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 999999);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == -5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == -1);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).unsafe() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).unsafe() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).unsafe() == 32);
    CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);
}

TEST_CASE("Jump to non existing label")
{
    // J
    res = dlx::Parser::Parse("J label");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::UnknownLabel);

    // JAL
    res = dlx::Parser::Parse("JAL label");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::UnknownLabel);
}

TEST_CASE("Invalid jump register")
{
    // JR
    res = dlx::Parser::Parse("JR R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, -5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    // JALR
    res = dlx::Parser::Parse("JALR R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, -5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

// Loading invalid address
TEST_CASE("Loading invalid address - LB")
{
    res = dlx::Parser::Parse("LB R1 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LB R1 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LB R1 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LB R1 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LB R1 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LB R1 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Loading invalid address - LBU")
{
    res = dlx::Parser::Parse("LBU R1 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LBU R1 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LBU R1 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LBU R1 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LBU R1 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Loading invalid address - LH")
{
    res = dlx::Parser::Parse("LH R1 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LH R1 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LH R1 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LH R1 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LH R1 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LH R1 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Loading invalid address - LHU")
{
    res = dlx::Parser::Parse("LHU R1 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LHU R1 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LHU R1 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LHU R1 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LHU R1 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LHU R1 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Loading invalid address - LW")
{
    res = dlx::Parser::Parse("LW R1 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Loading invalid address - LWU")
{
    res = dlx::Parser::Parse("LWU R1 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LWU R1 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LWU R1 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LWU R1 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LWU R1 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LWU R1 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Loading invalid address - LF")
{
    res = dlx::Parser::Parse("LF F0 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LF F0 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LF F0 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LF F0 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LF F0 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LF F0 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Loading invalid address - LD")
{
    res = dlx::Parser::Parse("LD F0 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LD F0 #-4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LD F0 #5000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LD F0 4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LD F0 -4(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LD F0 5000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

// Storing invalid address
TEST_CASE("Storing invalid address - SB")
{
    res = dlx::Parser::Parse("SB #4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SB #-4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SB #5000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SB 4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SB -4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SB 5000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Storing invalid address - SBU")
{
    res = dlx::Parser::Parse("SBU #4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SBU #-4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SBU #5000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SBU 4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SBU -4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SBU 5000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Storing invalid address - SH")
{
    res = dlx::Parser::Parse("SH #4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SH #-4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SH #5000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SH 4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SH -4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SH 5000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Storing invalid address - SHU")
{
    res = dlx::Parser::Parse("SHU #4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SHU #-4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SHU #5000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SHU 4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SHU -4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SHU 5000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Storing invalid address - SW")
{
    res = dlx::Parser::Parse("SW #4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SW #-4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SW #5000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SW 4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SW -4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SW 5000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Storing invalid address - SWU")
{
    res = dlx::Parser::Parse("SWU #4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SWU #-4 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SWU #5000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SWU 4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SWU -4(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SWU 5000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Storing invalid address - SF")
{
    res = dlx::Parser::Parse("SF #4 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SF #-4 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SF #5000 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SF 4(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SF -4(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SF 5000(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Storing invalid address - SD")
{
    res = dlx::Parser::Parse("SD #4 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SD #-4 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SD #5000 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SD 4(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SD -4(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("SD 5000(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("MisalignedRegisterAccess")
{
    // Iterate over all odd registers
    for (phi::usize i{1u}; i < 30u; i += 2u)
    {
        // Write to
        res = dlx::Parser::Parse(fmt::format("ADDD F{} F0 F0", i.unsafe()));
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);
        proc.ExecuteCurrentProgram();

        CHECK(proc.IsHalted());
        CHECK(proc.GetLastRaisedException() == dlx::Exception::MisalignedRegisterAccess);

        // Read from
        res = dlx::Parser::Parse(fmt::format("ADDD F0 F{} F0", i.unsafe()));
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);
        proc.ExecuteCurrentProgram();

        CHECK(proc.IsHalted());
        CHECK(proc.GetLastRaisedException() == dlx::Exception::MisalignedRegisterAccess);
    }
}

// Other tests

TEST_CASE("R0 is read only")
{
    res = dlx::Parser::Parse("ADDI R0 R0 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);
    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R0).unsafe() == 0);

    // Unsigned
    res = dlx::Parser::Parse("ADDUI R0 R0 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);
    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R0).unsafe() == 0);
}

TEST_CASE("Empty source code")
{
    dlx::Processor processor;

    // Should be no ops
    processor.ExecuteCurrentProgram();
    processor.ExecuteStep();

    res = dlx::Parser::Parse("");
    REQUIRE(res.m_ParseErrors.empty());
    CHECK(res.m_Instructions.empty());
    CHECK(res.m_JumpData.empty());
    CHECK_FALSE(res.IsValid());

    processor.LoadProgram(res);

    processor.ExecuteCurrentProgram();
    processor.ExecuteStep();

    CHECK(processor.IsHalted());
}

TEST_CASE("Processor::LoadProgram")
{
    // Parser errors
    res = dlx::Parser::Parse("This has errors");
    REQUIRE_FALSE(res.m_ParseErrors.empty());
    REQUIRE_FALSE(res.IsValid());

    // Returns false on prog with Parser errors
    CHECK_FALSE(proc.LoadProgram(res));

    // Should be NOPs
    proc.ExecuteCurrentProgram();
    proc.ExecuteStep();

    // Empty
    res = dlx::Parser::Parse("");

    CHECK(proc.LoadProgram(res));

    // Should be NOPs
    proc.ExecuteCurrentProgram();
    proc.ExecuteStep();

    // Valid
    res = dlx::Parser::Parse("ADD R1 R1 R1");

    CHECK(proc.LoadProgram(res));
}

TEST_CASE("Processor::ClearRegisters")
{
    // Set all registers to non zero
    for (int i{0}; i < 31; ++i)
    {
        proc.IntRegisterSetSignedValue(static_cast<dlx::IntRegisterID>(i), 42);
        proc.FloatRegisterSetFloatValue(static_cast<dlx::FloatRegisterID>(i), 3.14f);
    }
    proc.SetFPSRValue(true);

    proc.ClearRegisters();

    // Test
    for (int i{0}; i < 31; ++i)
    {
        CHECK(bool(proc.IntRegisterGetSignedValue(static_cast<dlx::IntRegisterID>(i)) == 0));
        CHECK(bool(proc.FloatRegisterGetFloatValue(static_cast<dlx::FloatRegisterID>(i)).unsafe() ==
                   0.0f));
    }

    CHECK_FALSE(proc.GetFPSRValue());
}

TEST_CASE("Processor::ClearMemory")
{
    using namespace phi::literals;

    auto& mem = proc.GetMemory();

    phi::usize start_adr = mem.GetStartingAddress();

    // Set all values to non zero
    for (phi::usize i{0u}; i < mem.GetSize(); ++i)
    {
        mem.StoreByte(start_adr + i, 1_i8);
    }

    proc.ClearMemory();

    // Test all values
    for (phi::usize i{0u}; i < mem.GetSize(); ++i)
    {
        CHECK(mem.LoadByte(start_adr + i)->unsafe() == 0);
    }
}

TEST_CASE("Misaligned addresses - Crash-8cb7670c0bacefed7af9ea62bcb5a03b95296b8e")
{
    // Signed half words
    res = dlx::Parser::Parse("LH R1 #1001");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LH R1 #1003");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    // Unsigned half words
    res = dlx::Parser::Parse("LHU R1 #1001");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LHU R1 #1003");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    // Signed words
    res = dlx::Parser::Parse("LW R1 #1001");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 #1002");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LW R1 #1003");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    // Unsigned words
    res = dlx::Parser::Parse("LWU R1 #1001");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LWU R1 #1002");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

    res = dlx::Parser::Parse("LWU R1 #1003");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
    CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
}

TEST_CASE("Dump functions")
{
    // Empty
    dlx::Processor processor;

    CHECK_FALSE(processor.GetRegisterDump().empty());
    CHECK_FALSE(processor.GetMemoryDump().empty());
    CHECK_FALSE(processor.GetProcessorDump().empty());
    CHECK_FALSE(processor.GetCurrentProgrammDump().empty());

    // Single instruction
    res = dlx::Parser::Parse("ADD R1 R1 R1");
    REQUIRE(res.m_ParseErrors.empty());
    REQUIRE(res.IsValid());

    processor.LoadProgram(res);

    CHECK_FALSE(processor.GetRegisterDump().empty());
    CHECK_FALSE(processor.GetMemoryDump().empty());
    CHECK_FALSE(processor.GetProcessorDump().empty());
    CHECK_FALSE(processor.GetCurrentProgrammDump().empty());

    // Jump labels
    res = dlx::Parser::Parse("a: ADD R1 R1 R1\nb: NOP\nJ a");
    REQUIRE(res.m_ParseErrors.empty());
    REQUIRE(res.IsValid());

    processor.LoadProgram(res);

    CHECK_FALSE(processor.GetRegisterDump().empty());
    CHECK_FALSE(processor.GetMemoryDump().empty());
    CHECK_FALSE(processor.GetProcessorDump().empty());
    CHECK_FALSE(processor.GetCurrentProgrammDump().empty());

    // Parse error
    res = dlx::Parser::Parse("DLX with Parse error");
    REQUIRE_FALSE(res.m_ParseErrors.empty());
    REQUIRE_FALSE(res.IsValid());

    processor.LoadProgram(res);

    CHECK_FALSE(processor.GetRegisterDump().empty());
    CHECK_FALSE(processor.GetMemoryDump().empty());
    CHECK_FALSE(processor.GetProcessorDump().empty());
    CHECK_FALSE(processor.GetCurrentProgrammDump().empty());
}
