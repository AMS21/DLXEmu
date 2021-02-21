#include "DLX/RegisterNames.hpp"
#include <catch2/catch_test_macros.hpp>

#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>

TEST_CASE("Operation exceptions")
{
    dlx::InstructionLibrary lib;
    dlx::Processor          proc;
    dlx::ParsedProgram      res;

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

    SECTION("Storing invalid address")
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
}

TEST_CASE("Processor instructions")
{
    dlx::InstructionLibrary lib;
    dlx::Processor          proc;
    dlx::ParsedProgram      res;

    SECTION("ADD")
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

    SECTION("ADDI")
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

    SECTION("ADDU")
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

    SECTION("ADDUI")
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

    SECTION("ADDF")
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

    SECTION("ADDD")
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

    SECTION("SUB")
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

    SECTION("SUBI")
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

    SECTION("SUBU")
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

    SECTION("SUBUI")
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

    SECTION("SUBF")
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

    SECTION("SUBD")
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

    SECTION("MULT")
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

    SECTION("MULTI")
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

    SECTION("MULTU")
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

    SECTION("MULTUI")
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

    SECTION("MULTF")
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

    SECTION("MULTD")
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

    SECTION("DIV")
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

    SECTION("DIVI")
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

    SECTION("DIVU")
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

    SECTION("DIVUI")
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

    SECTION("DIVF")
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

    SECTION("DIVD")
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

    SECTION("SLL")
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
    }

    SECTION("SLLI")
    {
        res = dlx::Parser::Parse(lib, "SLLI R1 R2 #2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 32);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    }

    SECTION("SRL")
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
    }

    SECTION("SRLI")
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
    }

    SECTION("SLA")
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
    }

    SECTION("SLAI")
    {
        res = dlx::Parser::Parse(lib, "SLAI R1 R2 #2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);
        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R2, 8);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 32);
        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R2).get() == 8);
    }

    SECTION("SRA")
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
    }

    SECTION("SRAI")
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
    }

    SECTION("AND")
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

    SECTION("ANDI")
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

    SECTION("OR")
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

    SECTION("ORI")
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

    SECTION("XOR")
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

    SECTION("XORI")
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

    SECTION("SLT")
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

    SECTION("SLTI")
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

    SECTION("LTF")
    {
        res = dlx::Parser::Parse(lib, "LTF F0 F1 F2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
    }

    SECTION("LTD")
    {
        res = dlx::Parser::Parse(lib, "LTD F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);
    }

    SECTION("SGT")
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

    SECTION("SGTI")
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

    SECTION("GTF")
    {
        res = dlx::Parser::Parse(lib, "GTF F0 F1 F2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);
    }

    SECTION("GTD")
    {
        res = dlx::Parser::Parse(lib, "GTD F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
    }

    SECTION("SLE")
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

    SECTION("SLEI")
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

    SECTION("LEF")
    {
        res = dlx::Parser::Parse(lib, "LEF F0 F1 F2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
    }

    SECTION("LED")
    {
        res = dlx::Parser::Parse(lib, "LED F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);
    }

    SECTION("SGE")
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

    SECTION("SGEI")
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

    SECTION("GEF")
    {
        res = dlx::Parser::Parse(lib, "GEF F0 F1 F2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);
    }

    SECTION("GED")
    {
        res = dlx::Parser::Parse(lib, "GED F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
    }

    SECTION("SEQ")
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

    SECTION("SEQI")
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

    SECTION("EQF")
    {
        res = dlx::Parser::Parse(lib, "EQF F0 F1 F2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 2.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 2.0f);
    }

    SECTION("EQD")
    {
        res = dlx::Parser::Parse(lib, "EQD F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 2.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 2.0);
    }

    SECTION("SNE")
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

    SECTION("SNEI")
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

    SECTION("NEF")
    {
        res = dlx::Parser::Parse(lib, "NEF F0 F1 F2");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 2.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 2.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F1, 1.0f);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F2, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F1).get() == 1.0f);
        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F2).get() == 1.0f);
    }

    SECTION("NED")
    {
        res = dlx::Parser::Parse(lib, "NED F0 F2 F4");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 2.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 2.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F2, 1.0);
        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F4, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 0.0f);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F2).get() == 1.0);
        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F4).get() == 1.0);
    }

    SECTION("BEQZ")
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

    SECTION("BNEZ")
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

    SECTION("BFPT")
    {
        const char* data =
                R"(
            BFPT F0 true
            HALT
        true:
            ADDI R1 R0 #1
        )";
        res = dlx::Parser::Parse(lib, data);
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 0.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    }

    SECTION("BFPF")
    {
        const char* data =
                R"(
            BFPF F0 false
            HALT
        false:
            ADDI R1 R0 #1
        )";
        res = dlx::Parser::Parse(lib, data);
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 0.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 1);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 0);
        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 0);
    }

    SECTION("J")
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

    SECTION("JR")
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

    SECTION("JAL")
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

    SECTION("JALR")
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

    SECTION("LB")
    {
        res = dlx::Parser::Parse(lib, "LB R1 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreByte(1000u, static_cast<std::int8_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);

        res = dlx::Parser::Parse(lib, "LB R1 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreByte(1000u, static_cast<std::int8_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);
    }

    SECTION("LBU")
    {
        res = dlx::Parser::Parse(lib, "LBU R1 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreUnsignedByte(1000u, static_cast<std::uint8_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);

        res = dlx::Parser::Parse(lib, "LBU R1 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreUnsignedByte(1000u, static_cast<std::uint8_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21);
    }

    SECTION("LH")
    {
        res = dlx::Parser::Parse(lib, "LH R1 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreHalfWord(1000u, static_cast<std::int16_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);

        res = dlx::Parser::Parse(lib, "LH R1 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreHalfWord(1000u, static_cast<std::int16_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);
    }

    SECTION("LHU")
    {
        res = dlx::Parser::Parse(lib, "LHU R1 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreUnsignedHalfWord(1000u, static_cast<std::uint16_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);

        res = dlx::Parser::Parse(lib, "LHU R1 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreUnsignedHalfWord(1000u, static_cast<std::uint16_t>(21));

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21);
    }

    SECTION("LW")
    {
        res = dlx::Parser::Parse(lib, "LW R1 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreWord(1000u, 21);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);

        res = dlx::Parser::Parse(lib, "LW R1 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 9999999);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreWord(1000u, 21);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetSignedValue(dlx::IntRegisterID::R1).get() == 21);
    }

    SECTION("LWU")
    {
        res = dlx::Parser::Parse(lib, "LWU R1 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreUnsignedWord(1000u, 21u);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21u);

        res = dlx::Parser::Parse(lib, "LWU R1 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 9999999u);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreUnsignedWord(1000u, 21u);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IntRegisterGetUnsignedValue(dlx::IntRegisterID::R1).get() == 21);
    }

    SECTION("LF")
    {
        res = dlx::Parser::Parse(lib, "LF F0 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreFloat(1000u, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);

        res = dlx::Parser::Parse(lib, "LF F0 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, -1.0f);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreFloat(1000u, 1.0f);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetFloatValue(dlx::FloatRegisterID::F0).get() == 1.0f);
    }

    SECTION("LD")
    {
        res = dlx::Parser::Parse(lib, "LD F0 #1000");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreDouble(1000u, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 1.0);

        res = dlx::Parser::Parse(lib, "LD F0 1000(R0)");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, -1.0);

        proc.ClearMemory();
        proc.m_MemoryBlock.StoreDouble(1000u, 1.0);

        proc.ExecuteCurrentProgram();

        CHECK(proc.FloatRegisterGetDoubleValue(dlx::FloatRegisterID::F0).get() == 1.0);
    }

    SECTION("SB")
    {
        res = dlx::Parser::Parse(lib, "SB #1000 R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadByte(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21);

        res = dlx::Parser::Parse(lib, "SB 1000(R0) R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadByte(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21);
    }

    SECTION("SBU")
    {
        res = dlx::Parser::Parse(lib, "SBU #1000 R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadUnsignedByte(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21u);

        res = dlx::Parser::Parse(lib, "SBU 1000(R0) R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadUnsignedByte(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21u);
    }

    SECTION("SH")
    {
        res = dlx::Parser::Parse(lib, "SH #1000 R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadHalfWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21);

        res = dlx::Parser::Parse(lib, "SH 1000(R0) R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadHalfWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21);
    }

    SECTION("SHU")
    {
        res = dlx::Parser::Parse(lib, "SHU #1000 R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadUnsignedHalfWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21u);

        res = dlx::Parser::Parse(lib, "SHU 1000(R0) R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadUnsignedHalfWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21u);
    }

    SECTION("SW")
    {
        res = dlx::Parser::Parse(lib, "SW #1000 R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21);

        res = dlx::Parser::Parse(lib, "SW 1000(R0) R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetSignedValue(dlx::IntRegisterID::R1, 21);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21);
    }

    SECTION("SWU")
    {
        res = dlx::Parser::Parse(lib, "SWU #1000 R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadUnsignedWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21u);

        res = dlx::Parser::Parse(lib, "SWU 1000(R0) R1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.IntRegisterSetUnsignedValue(dlx::IntRegisterID::R1, 21u);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadUnsignedWord(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 21u);
    }

    SECTION("SF")
    {
        res = dlx::Parser::Parse(lib, "SF #1000 F0");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadFloat(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 1.0f);

        res = dlx::Parser::Parse(lib, "SF 1000(R0) F0");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetFloatValue(dlx::FloatRegisterID::F0, 1.0f);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadFloat(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 1.0f);
    }

    SECTION("SD")
    {
        res = dlx::Parser::Parse(lib, "SD #1000 F0");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, 1.0);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        auto val = proc.m_MemoryBlock.LoadDouble(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 1.0);

        res = dlx::Parser::Parse(lib, "SD 1000(R0) F0");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.FloatRegisterSetDoubleValue(dlx::FloatRegisterID::F0, 1.0);
        proc.ClearMemory();

        proc.ExecuteCurrentProgram();

        val = proc.m_MemoryBlock.LoadDouble(1000u);
        REQUIRE(val.has_value());
        CHECK(val->get() == 1.0);
    }

    SECTION("TRAP")
    {
        res = dlx::Parser::Parse(lib, "TRAP #1");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IsHalted());
    }

    SECTION("HALT")
    {
        res = dlx::Parser::Parse(lib, "HALT");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.ExecuteCurrentProgram();

        CHECK(proc.IsHalted());
    }

    SECTION("NOP")
    {
        res = dlx::Parser::Parse(lib, "NOP");
        REQUIRE(res.m_ParseErrors.empty());

        proc.LoadProgram(res);

        proc.ExecuteCurrentProgram();

        CHECK_FALSE(proc.IsHalted());
    }
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
