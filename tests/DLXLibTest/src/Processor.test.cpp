#include "DLX/RegisterNames.hpp"
#include <catch2/catch_test_macros.hpp>

#include <DLX/InstructionLibrary.hpp>
#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>

static dlx::InstructionLibrary lib;
static dlx::Processor          proc;
static dlx::ParsedProgram      res;

TEST_CASE("Operation exceptions")
{
    constexpr std::int32_t  signed_min   = phi::i32::limits_type::min();
    constexpr std::int32_t  signed_max   = phi::i32::limits_type::max();
    constexpr std::uint32_t unsigned_max = phi::u32::limits_type::max();

    SECTION("Signed addition overflow")
    {
        res = dlx::Parser::Parse(lib, "ADD R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 5);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_min + 4);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 5);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
    }

    SECTION("Signed addition underflow")
    {
        res = dlx::Parser::Parse(lib, "ADD R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == -1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -2);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max - 1);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == -2);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
    }

    SECTION("Unsigned addition overflow")
    {
        res = dlx::Parser::Parse(lib, "ADDU R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, unsigned_max);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == unsigned_max);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == unsigned_max);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 0u);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, unsigned_max);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 4u);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == unsigned_max);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 5u);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
    }

    SECTION("Signed subtraction overflow")
    {
        res = dlx::Parser::Parse(lib, "SUB R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -5);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_min + 4);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == -5);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
    }

    SECTION("Signed subtraction underflow")
    {
        res = dlx::Parser::Parse(lib, "SUB R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max - 1);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 2);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
    }

    SECTION("Unsigned subtraction underflow")
    {
        res = dlx::Parser::Parse(lib, "SUBU R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 0u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 0u);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 0u);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 0u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 1u);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == unsigned_max);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 0u);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 1u);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 0u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == unsigned_max - 4u);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 0u);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 5u);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
    }

    SECTION("Signed multiplication overflow")
    {
        res = dlx::Parser::Parse(lib, "MULT R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::None);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

        proc.ExecuteCurrentProgram();

        //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == -2);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 2);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, signed_max);

        proc.ExecuteCurrentProgram();

        //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == signed_max);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
    }

    SECTION("Signed multiplication underflow")
    {
        res = dlx::Parser::Parse(lib, "MULT R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_min);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

        proc.ExecuteCurrentProgram();

        //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, signed_max);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, signed_min);

        proc.ExecuteCurrentProgram();

        //CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == signed_min);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == signed_max);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == signed_min);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Underflow);
    }

    SECTION("Unsigned multiplication overflow")
    {
        res = dlx::Parser::Parse(lib, "MULTU R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, unsigned_max);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

        proc.ExecuteCurrentProgram();

        //CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == unsigned_max);
        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::Overflow);
    }

    SECTION("Signed division by zero")
    {
        res = dlx::Parser::Parse(lib, "DIV R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

        proc.ExecuteCurrentProgram();
        CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
        CHECK(proc.IsHalted());

        res = dlx::Parser::Parse(lib, "DIVI R1 R2 #0");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);

        proc.ExecuteCurrentProgram();
        CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
        CHECK(proc.IsHalted());
    }

    SECTION("Unsigned division by zero")
    {
        res = dlx::Parser::Parse(lib, "DIVU R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

        proc.ExecuteCurrentProgram();
        CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
        CHECK(proc.IsHalted());

        res = dlx::Parser::Parse(lib, "DIVUI R1 R2 #0");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 999999u);
        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);

        proc.ExecuteCurrentProgram();
        CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
        CHECK(proc.IsHalted());
    }

    SECTION("Float division by zero")
    {
        res = dlx::Parser::Parse(lib, "DIVF F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F4, 0.0f);

        proc.ExecuteCurrentProgram();
        CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
        CHECK(proc.IsHalted());
    }

    SECTION("Double division by zero")
    {
        res = dlx::Parser::Parse(lib, "DIVD F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 0.0f);

        proc.ExecuteCurrentProgram();
        CHECK(proc.GetLastRaisedException() == dlx::Exception::DivideByZero);
        CHECK(proc.IsHalted());
    }

    SECTION("Shift left bad shift")
    {
        // Logical
        res = dlx::Parser::Parse(lib, "SLL R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 32);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 999999);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == -1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

        // Arithmetic
        res = dlx::Parser::Parse(lib, "SLA R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 32);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 999999);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == -1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);
    }

    SECTION("Shift right bad shift")
    {
        // Logical
        res = dlx::Parser::Parse(lib, "SRL R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 32);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 999999);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == -1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

        // Arithmetic
        res = dlx::Parser::Parse(lib, "SRA R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 32);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == -1);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 32);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -5);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, -1);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 999999);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -5);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == -1);
        CHECK(proc.GetLastRaisedException() == dlx::Exception::BadShift);
    }

    SECTION("Jump to non existing label")
    {
        // J
        res = dlx::Parser::Parse(lib, "J label");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IsHalted());
        CHECK(proc.GetLastRaisedException() == dlx::Exception::UnknownLabel);

        // JAL
        res = dlx::Parser::Parse(lib, "JAL label");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IsHalted());
        CHECK(proc.GetLastRaisedException() == dlx::Exception::UnknownLabel);
    }

    SECTION("Invalid jump register")
    {
        // JR
        res = dlx::Parser::Parse(lib, "JR R1");
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
        res = dlx::Parser::Parse(lib, "JALR R1");
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

    SECTION("Loading invalid address")
    {
        SECTION("LB")
        {
            res = dlx::Parser::Parse(lib, "LB R1 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LB R1 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LB R1 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LB R1 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LB R1 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LB R1 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("LBU")
        {
            res = dlx::Parser::Parse(lib, "LBU R1 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LBU R1 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LBU R1 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LBU R1 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LBU R1 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LW R1 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("LH")
        {
            res = dlx::Parser::Parse(lib, "LH R1 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LH R1 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LH R1 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LH R1 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LH R1 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LH R1 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("LHU")
        {
            res = dlx::Parser::Parse(lib, "LHU R1 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LHU R1 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LHU R1 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LHU R1 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LHU R1 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LHU R1 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("LW")
        {
            res = dlx::Parser::Parse(lib, "LW R1 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LW R1 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LW R1 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LW R1 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LW R1 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LW R1 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("LWU")
        {
            res = dlx::Parser::Parse(lib, "LWU R1 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LWU R1 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LWU R1 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LWU R1 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LWU R1 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LWU R1 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("LF")
        {
            res = dlx::Parser::Parse(lib, "LF F0 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LF F0 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LF F0 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LF F0 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LF F0 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LF F0 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("LD")
        {
            res = dlx::Parser::Parse(lib, "LD F0 #4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LD F0 #-4");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LD F0 #5000");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LD F0 4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LD F0 -4(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "LD F0 5000(R0)");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }
    }

    SECTION("Storing invalid address")
    {
        SECTION("SB")
        {
            res = dlx::Parser::Parse(lib, "SB #4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SB #-4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SB #5000 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SB 4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SB -4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SB 5000(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("SBU")
        {
            res = dlx::Parser::Parse(lib, "SBU #4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SBU #-4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SBU #5000 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SBU 4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SBU -4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SBU 5000(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("SH")
        {
            res = dlx::Parser::Parse(lib, "SH #4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SH #-4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SH #5000 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SH 4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SH -4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SH 5000(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("SHU")
        {
            res = dlx::Parser::Parse(lib, "SHU #4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SHU #-4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SHU #5000 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SHU 4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SHU -4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SHU 5000(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("SW")
        {
            res = dlx::Parser::Parse(lib, "SW #4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SW #-4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SW #5000 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SW 4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SW -4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SW 5000(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("SWU")
        {
            res = dlx::Parser::Parse(lib, "SWU #4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SWU #-4 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SWU #5000 R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SWU 4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SWU -4(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SWU 5000(R0) R1");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("SF")
        {
            res = dlx::Parser::Parse(lib, "SF #4 F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SF #-4 F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SF #5000 F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SF 4(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SF -4(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SF 5000(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }

        SECTION("SD")
        {
            res = dlx::Parser::Parse(lib, "SD #4 F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SD #-4 F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SD #5000 F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SD 4(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SD -4(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);

            res = dlx::Parser::Parse(lib, "SD 5000(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());

            proc.LoadProgram(res);

            proc.ExecuteCurrentProgram();

            CHECK(proc.IsHalted());
            CHECK(proc.GetLastRaisedException() == dlx::Exception::AddressOutOfBounds);
        }
    }
}

TEST_CASE("ADD")
{
    res = dlx::Parser::Parse(lib, "ADD R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 6);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 6);
}

TEST_CASE("ADDI")
{
    res = dlx::Parser::Parse(lib, "ADDI R1 R2 #30");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 32);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
}

TEST_CASE("ADDU")
{
    res = dlx::Parser::Parse(lib, "ADDU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 19u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 19u);
}

TEST_CASE("ADDUI")
{
    res = dlx::Parser::Parse(lib, "ADDUI R1 R2 #19");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
}

TEST_CASE("ADDF")
{
    res = dlx::Parser::Parse(lib, "ADDF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f + 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).get() == 2.0f);
}

TEST_CASE("ADDD")
{
    res = dlx::Parser::Parse(lib, "ADDD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 1.0 + 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
}

TEST_CASE("SUB")
{
    res = dlx::Parser::Parse(lib, "SUB R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 50);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 30);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 20);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 50);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 30);
}

TEST_CASE("SUBI")
{
    res = dlx::Parser::Parse(lib, "SUBI R1 R2 #25");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 50);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 25);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 50);
}

TEST_CASE("SUBU")
{
    res = dlx::Parser::Parse(lib, "SUBU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 50u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 30u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 20u);
}

TEST_CASE("SUBUI")
{
    res = dlx::Parser::Parse(lib, "SUBUI R1 R2 #25");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 50u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 25u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 50u);
}

TEST_CASE("SUBF")
{
    res = dlx::Parser::Parse(lib, "SUBF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f - 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).get() == 1.0f);
}

TEST_CASE("SUBD")
{
    res = dlx::Parser::Parse(lib, "SUBD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 2.0 - 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);
}

TEST_CASE("MULT")
{
    res = dlx::Parser::Parse(lib, "MULT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("MULTI")
{
    res = dlx::Parser::Parse(lib, "MULTI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
}

TEST_CASE("MULTU")
{
    res = dlx::Parser::Parse(lib, "MULTU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 6u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);
}

TEST_CASE("MULTUI")
{
    res = dlx::Parser::Parse(lib, "MULTUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 6u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
}

TEST_CASE("MULTF")
{
    res = dlx::Parser::Parse(lib, "MULTF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 3.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 3.0f * 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 3.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).get() == 2.0f);
}

TEST_CASE("MULTD")
{
    res = dlx::Parser::Parse(lib, "MULTD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 3.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 3.0 * 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 3.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
}

TEST_CASE("DIV")
{
    res = dlx::Parser::Parse(lib, "DIV R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 6);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 2);

    // Divide by zero
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("DIVI")
{
    res = dlx::Parser::Parse(lib, "DIVI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 6);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 6);

    // Divide by zero
    res = dlx::Parser::Parse(lib, "DIVI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 6);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("DIVU")
{
    res = dlx::Parser::Parse(lib, "DIVU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 6u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 6u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 2u);

    // Divide by zero
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 0u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("DIVUI")
{
    res = dlx::Parser::Parse(lib, "DIVUI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 6u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 6u);

    // Divide by zero

    res = dlx::Parser::Parse(lib, "DIVUI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();
    CHECK(proc.IsHalted());
}

TEST_CASE("DIVF")
{
    res = dlx::Parser::Parse(lib, "DIVF F1 F2 F3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 12.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F3, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 12.0f / 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 12.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F3).get() == 2.0f);
}

TEST_CASE("DIVD")
{
    res = dlx::Parser::Parse(lib, "DIVD F0 F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 12.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 12.0 / 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 12.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
}

TEST_CASE("SLL")
{
    res = dlx::Parser::Parse(lib, "SLL R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 16);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
}

TEST_CASE("SLLI")
{
    res = dlx::Parser::Parse(lib, "SLLI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 32);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);

    res = dlx::Parser::Parse(lib, "SLLI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
}

TEST_CASE("SRL")
{
    res = dlx::Parser::Parse(lib, "SRL R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 4);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 2147483647);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == -1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
}

TEST_CASE("SRLI")
{
    res = dlx::Parser::Parse(lib, "SRLI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1073741823);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -1);

    res = dlx::Parser::Parse(lib, "SRLI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
}

TEST_CASE("SLA")
{
    res = dlx::Parser::Parse(lib, "SLA R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 16);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
}

TEST_CASE("SLAI")
{
    res = dlx::Parser::Parse(lib, "SLAI R1 R2 #2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 32);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);

    res = dlx::Parser::Parse(lib, "SLAI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
}

TEST_CASE("SRA")
{
    res = dlx::Parser::Parse(lib, "SRA R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   -2147483647); // 0b10000000'00000000'00000000'00000001
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    // 0b11000000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == -1073741824);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -2147483647);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   1073741825); // 0b01000000'00000000'00000000'00000001
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 1);

    proc.ExecuteCurrentProgram();

    // 0b00100000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 536870912);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1073741825);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 0);
}

TEST_CASE("SRAI")
{
    res = dlx::Parser::Parse(lib, "SRAI R1 R2 #1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   -2147483647); // 0b10000000'00000000'00000000'00000001

    proc.ExecuteCurrentProgram();

    // 0b11000000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == -1073741824);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == -2147483647);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2,
                                   1073741825); // 0b01000000'00000000'00000000'00000001

    proc.ExecuteCurrentProgram();

    // 0b00100000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 536870912);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1073741825);

    res = dlx::Parser::Parse(lib, "SRAI R1 R2 #0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

    proc.ExecuteCurrentProgram();

    // 0b11000000'00000000'00000000'00000000
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 8);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
}

TEST_CASE("AND")
{
    res = dlx::Parser::Parse(lib, "AND R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("ANDI")
{
    res = dlx::Parser::Parse(lib, "ANDI R1 R2 #5");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
}

TEST_CASE("OR")
{
    res = dlx::Parser::Parse(lib, "OR R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("ORI")
{
    res = dlx::Parser::Parse(lib, "ORI R1 R2 #8");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 9);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
}

TEST_CASE("XOR")
{
    res = dlx::Parser::Parse(lib, "XOR R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 7);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 7);
}

TEST_CASE("XORI")
{
    res = dlx::Parser::Parse(lib, "XORI R1 R2 #7");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 6);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
}

TEST_CASE("SLT")
{
    res = dlx::Parser::Parse(lib, "SLT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 5);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("SLTI")
{
    res = dlx::Parser::Parse(lib, "SLTI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 4);
}

TEST_CASE("SLTU")
{
    res = dlx::Parser::Parse(lib, "SLTU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 5u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 5u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);
}

TEST_CASE("SLTUI")
{
    res = dlx::Parser::Parse(lib, "SLTUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 4u);
}

TEST_CASE("LTF")
{
    res = dlx::Parser::Parse(lib, "LTF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
}

TEST_CASE("LTD")
{
    res = dlx::Parser::Parse(lib, "LTD F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);
}

TEST_CASE("SGT")
{
    res = dlx::Parser::Parse(lib, "SGT R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("SGTI")
{
    res = dlx::Parser::Parse(lib, "SGTI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 4);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
}

TEST_CASE("SGTU")
{
    res = dlx::Parser::Parse(lib, "SGTU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);
}

TEST_CASE("SGTUI")
{
    res = dlx::Parser::Parse(lib, "SGTUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 4u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
}

TEST_CASE("GTF")
{
    res = dlx::Parser::Parse(lib, "GTF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);
}
TEST_CASE("GTD")
{
    res = dlx::Parser::Parse(lib, "GTD F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
}
TEST_CASE("SLE")
{
    res = dlx::Parser::Parse(lib, "SLE R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 5);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("SLEI")
{
    res = dlx::Parser::Parse(lib, "SLEI R1 R2 #3");

    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 4);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
}

TEST_CASE("SLEU")
{
    res = dlx::Parser::Parse(lib, "SLEU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 5u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 5u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);
}

TEST_CASE("SLEUI")
{
    res = dlx::Parser::Parse(lib, "SLEUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 4u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
}

TEST_CASE("LEF")
{
    res = dlx::Parser::Parse(lib, "LEF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
}

TEST_CASE("LED")
{
    res = dlx::Parser::Parse(lib, "LED F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);
}

TEST_CASE("SGE")
{
    res = dlx::Parser::Parse(lib, "SGE R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 5);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 5);
}

TEST_CASE("SGEI")
{
    res = dlx::Parser::Parse(lib, "SGEI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 4);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 4);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
}

TEST_CASE("SGEU")
{
    res = dlx::Parser::Parse(lib, "SGEU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 5u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 5u);
}

TEST_CASE("SGEUI")
{
    res = dlx::Parser::Parse(lib, "SGEUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 4u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 4u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
}

TEST_CASE("GEF")
{
    res = dlx::Parser::Parse(lib, "GEF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);
}

TEST_CASE("GED")
{
    res = dlx::Parser::Parse(lib, "GED F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
}

TEST_CASE("SEQ")
{
    res = dlx::Parser::Parse(lib, "SEQ R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("SEQI")
{
    res = dlx::Parser::Parse(lib, "SEQI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
}

TEST_CASE("SEQU")
{
    res = dlx::Parser::Parse(lib, "SEQU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);
}

TEST_CASE("SEQUI")
{
    res = dlx::Parser::Parse(lib, "SEQUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
}

TEST_CASE("EQF")
{
    res = dlx::Parser::Parse(lib, "EQF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);
}

TEST_CASE("EQD")
{
    res = dlx::Parser::Parse(lib, "EQD F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
}

TEST_CASE("SNE")
{
    res = dlx::Parser::Parse(lib, "SNE R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R3).get() == 3);
}

TEST_CASE("SNEI")
{
    res = dlx::Parser::Parse(lib, "SNEI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R3, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 2);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 3);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 3);
}

TEST_CASE("SNEU")
{
    res = dlx::Parser::Parse(lib, "SNEU R1 R2 R3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R3).get() == 3u);
}

TEST_CASE("SNEUI")
{
    res = dlx::Parser::Parse(lib, "SNEUI R1 R2 #3");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 2u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R3, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 1u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 2u);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);
    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R2, 3u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 0u);
    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R2).get() == 3u);
}

TEST_CASE("NEF")
{
    res = dlx::Parser::Parse(lib, "NEF F1 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
}

TEST_CASE("NED")
{
    res = dlx::Parser::Parse(lib, "NED F2 F4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.SetFPSRValue(false);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

    proc.SetFPSRValue(true);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.GetFPSRValue());
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);
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
    res = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 0);
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
    res = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 5);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 0);
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
    res = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(true);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(false);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
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
    res = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(false);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
    proc.SetFPSRValue(true);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
}

TEST_CASE("J")
{
    const char* data = R"(
            J jump_label
            HALT

        jump_label:
            ADDI R1 R0 #1
        )";
    res              = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
}

TEST_CASE("JR")
{
    const char* data = R"(
            JR R1
            HALT

            ADDI R2 R0 #1
        )";

    res = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
}

TEST_CASE("JAL")
{
    const char* data = R"(
            JAL jump_label
            HALT

        jump_label:
            ADDI R1 R0 #1
        )";
    res              = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R31, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R31).get() == 1);
}

TEST_CASE("JALR")
{
    const char* data = R"(
            JALR R1
            HALT

            ADDI R2 R0 #1
        )";

    res = dlx::Parser::Parse(lib, data);
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 2);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 2);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 1);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R31).get() == 1);
}

TEST_CASE("LHI")
{
    res = dlx::Parser::Parse(lib, "LHI R1 #1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == (1 << 16));
}

TEST_CASE("LB")
{
    res = dlx::Parser::Parse(lib, "LB R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreByte(1000u, static_cast<std::int8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);

    res = dlx::Parser::Parse(lib, "LB R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreByte(1000u, static_cast<std::int8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);
}

TEST_CASE("LBU")
{
    res = dlx::Parser::Parse(lib, "LBU R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedByte(1000u, static_cast<std::uint8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);

    res = dlx::Parser::Parse(lib, "LBU R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedByte(1000u, static_cast<std::uint8_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21);
}

TEST_CASE("LH")
{
    res = dlx::Parser::Parse(lib, "LH R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreHalfWord(1000u, static_cast<std::int16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);

    res = dlx::Parser::Parse(lib, "LH R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreHalfWord(1000u, static_cast<std::int16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);
}

TEST_CASE("LHU")
{
    res = dlx::Parser::Parse(lib, "LHU R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedHalfWord(1000u, static_cast<std::uint16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);

    res = dlx::Parser::Parse(lib, "LHU R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedHalfWord(1000u, static_cast<std::uint16_t>(21));

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21);
}

TEST_CASE("LW")
{
    res = dlx::Parser::Parse(lib, "LW R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreWord(1000u, 21);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);

    res = dlx::Parser::Parse(lib, "LW R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

    proc.ClearMemory();
    proc.GetMemory().StoreWord(1000u, 21);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);
}

TEST_CASE("LWU")
{
    res = dlx::Parser::Parse(lib, "LWU R1 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedWord(1000u, 21u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);

    res = dlx::Parser::Parse(lib, "LWU R1 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

    proc.ClearMemory();
    proc.GetMemory().StoreUnsignedWord(1000u, 21u);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21);
}

TEST_CASE("LF")
{
    res = dlx::Parser::Parse(lib, "LF F0 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

    proc.ClearMemory();
    proc.GetMemory().StoreFloat(1000u, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);

    res = dlx::Parser::Parse(lib, "LF F0 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

    proc.ClearMemory();
    proc.GetMemory().StoreFloat(1000u, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
}

TEST_CASE("LD")
{
    res = dlx::Parser::Parse(lib, "LD F0 #1000");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);

    proc.ClearMemory();
    proc.GetMemory().StoreDouble(1000u, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 1.0);

    res = dlx::Parser::Parse(lib, "LD F0 1000(R0)");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);

    proc.ClearMemory();
    proc.GetMemory().StoreDouble(1000u, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 1.0);
}

TEST_CASE("SB")
{
    res = dlx::Parser::Parse(lib, "SB #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21);

    res = dlx::Parser::Parse(lib, "SB 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21);
}

TEST_CASE("SBU")
{
    res = dlx::Parser::Parse(lib, "SBU #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadUnsignedByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21u);

    res = dlx::Parser::Parse(lib, "SBU 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadUnsignedByte(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21u);
}

TEST_CASE("SH")
{
    res = dlx::Parser::Parse(lib, "SH #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21);

    res = dlx::Parser::Parse(lib, "SH 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21);
}

TEST_CASE("SHU")
{
    res = dlx::Parser::Parse(lib, "SHU #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadUnsignedHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21u);

    res = dlx::Parser::Parse(lib, "SHU 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadUnsignedHalfWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21u);
}

TEST_CASE("SW")
{
    res = dlx::Parser::Parse(lib, "SW #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21);

    res = dlx::Parser::Parse(lib, "SW 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21);
}

TEST_CASE("SWU")
{
    res = dlx::Parser::Parse(lib, "SWU #1000 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadUnsignedWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21u);

    res = dlx::Parser::Parse(lib, "SWU 1000(R0) R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadUnsignedWord(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 21u);
}

TEST_CASE("SF")
{
    res = dlx::Parser::Parse(lib, "SF #1000 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadFloat(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 1.0f);

    res = dlx::Parser::Parse(lib, "SF 1000(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadFloat(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 1.0f);
}

TEST_CASE("SD")
{
    res = dlx::Parser::Parse(lib, "SD #1000 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, 1.0);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    auto val = proc.GetMemory().LoadDouble(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 1.0);

    res = dlx::Parser::Parse(lib, "SD 1000(R0) F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, 1.0);
    proc.ClearMemory();

    proc.ExecuteCurrentProgram();

    val = proc.GetMemory().LoadDouble(1000u);
    REQUIRE(val.has_value());
    CHECK(val->get() == 1.0);
}

TEST_CASE("MOVF")
{
    res = dlx::Parser::Parse(lib, "MOVF F0 F1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
}

TEST_CASE("MOVD")
{
    res = dlx::Parser::Parse(lib, "MOVD F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 1.0);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
}

TEST_CASE("MOVFP2I")
{
    res = dlx::Parser::Parse(lib, "MOVFP2I R1 F0");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, -1);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() != -1);
}

TEST_CASE("MOVI2FP")
{
    res = dlx::Parser::Parse(lib, "MOVI2FP F0 R1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 1);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() != -1.0f);
}

TEST_CASE("CVTF2D")
{
    res = dlx::Parser::Parse(lib, "CVTF2D F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 1.0);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
}

TEST_CASE("CVTF2I")
{
    res = dlx::Parser::Parse(lib, "CVTF2I F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() != -1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
}

TEST_CASE("CVTD2F")
{
    res = dlx::Parser::Parse(lib, "CVTD2F F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
}

TEST_CASE("CVTD2I")
{
    res = dlx::Parser::Parse(lib, "CVTD2I F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() != -1.0f);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
}

TEST_CASE("CVTI2F")
{
    res = dlx::Parser::Parse(lib, "CVTI2F F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() != -1.0f);
    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0);
}

TEST_CASE("CVTI2D")
{
    res = dlx::Parser::Parse(lib, "CVTI2D F0 F2");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
    proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0f);

    proc.ExecuteCurrentProgram();

    CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() != 1.0f);
    CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
}

TEST_CASE("TRAP")
{
    res = dlx::Parser::Parse(lib, "TRAP #1");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("HALT")
{
    res = dlx::Parser::Parse(lib, "HALT");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IsHalted());
}

TEST_CASE("NOP")
{
    res = dlx::Parser::Parse(lib, "NOP");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK_FALSE(proc.IsHalted());
}

TEST_CASE("R0 is read only")
{
    dlx::InstructionLibrary lib;
    dlx::Processor          proc;
    dlx::ParsedProgram      res;

    res = dlx::Parser::Parse(lib, "ADDI R0 R0 #4");
    REQUIRE(res.m_ParseErrors.empty());

    proc.LoadProgram(res);

    proc.ExecuteCurrentProgram();

    CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R0).get() == 0);
}
