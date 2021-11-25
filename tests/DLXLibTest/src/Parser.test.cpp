#include <catch2/catch_test_macros.hpp>

#include <DLX/Instruction.hpp>
#include <DLX/InstructionArg.hpp>
#include <DLX/InstructionLibrary.hpp>
#include <DLX/IntRegister.hpp>
#include <DLX/OpCode.hpp>
#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <DLX/RegisterNames.hpp>
#include <DLX/Token.hpp>

TEST_CASE("Only one instruction per line")
{
    dlx::ParsedProgram res;

    res = dlx::Parser::Parse("ADD R1 R2 R3 ADD R1 R2 R3");
    CHECK_FALSE(res.m_ParseErrors.empty());
}

[[nodiscard]] phi::Boolean InstructionMatches(const dlx::Instruction& instr, dlx::OpCode opcode,
                                              dlx::InstructionArg arg1, dlx::InstructionArg arg2,
                                              dlx::InstructionArg arg3)
{
    if (instr.GetInfo().GetOpCode() != opcode)
    {
        return false;
    }

    if (instr.GetArg1() != arg1)
    {
        return false;
    }

    if (instr.GetArg2() != arg2)
    {
        return false;
    }

    if (instr.GetArg3() != arg3)
    {
        return false;
    }

    return true;
}

TEST_CASE("InstructionMatches")
{
    dlx::ParsedProgram res;

    res = dlx::Parser::Parse("ADD, R2, R4, R29");
    REQUIRE(res.m_ParseErrors.empty());
    REQUIRE(res.m_Instructions.size() == 1);

    // Different opcode
    CHECK_FALSE(
            InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::ADDI,
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));

    // First arg wrong
    CHECK_FALSE(
            InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::ADD,
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R0),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));

    // Second arg wrong
    CHECK_FALSE(
            InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::ADD,
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R0),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));

    // Third arg wrong
    CHECK_FALSE(
            InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::ADD,
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                               dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R0)));
}

TEST_CASE("Parser")
{
    dlx::ParsedProgram res;

    SECTION("Empty string")
    {
        res = dlx::Parser::Parse("");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
        CHECK(res.m_ParseErrors.empty());
    }

    SECTION("Ignoring comments")
    {
        res = dlx::Parser::Parse("; Comment");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
        CHECK(res.m_ParseErrors.empty());

        res = dlx::Parser::Parse("/Comment");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
        CHECK(res.m_ParseErrors.empty());

        res = dlx::Parser::Parse("// First comment\n;Second comment");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
        CHECK(res.m_ParseErrors.empty());
    }

    SECTION("Ignoring new lines")
    {
        res = dlx::Parser::Parse("\n");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
        CHECK(res.m_ParseErrors.empty());

        res = dlx::Parser::Parse("\n\n\n\n");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
        CHECK(res.m_ParseErrors.empty());
    }

    SECTION("Jump labels")
    {
        res = dlx::Parser::Parse("start:");
        CHECK(res.m_Instructions.empty());
        REQUIRE_FALSE(res.m_ParseErrors.empty());

        REQUIRE(res.m_JumpData.size() == 1);
        REQUIRE(res.m_JumpData.find("start") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("start") == 0u);

        res = dlx::Parser::Parse("a:\nb:\nc: NOP");
        CHECK_FALSE(res.m_Instructions.empty());
        REQUIRE(res.m_ParseErrors.empty());
        REQUIRE(res.m_JumpData.size() == 3);

        REQUIRE(res.m_JumpData.find("a") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("a") == 0u);

        REQUIRE(res.m_JumpData.find("b") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("b") == 0u);

        REQUIRE(res.m_JumpData.find("c") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("c") == 0u);

        res = dlx::Parser::Parse("a:b:c: NOP");
        CHECK_FALSE(res.m_Instructions.empty());
        REQUIRE(res.m_ParseErrors.empty());
        REQUIRE(res.m_JumpData.size() == 3);

        REQUIRE(res.m_JumpData.find("a") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("a") == 0u);

        REQUIRE(res.m_JumpData.find("b") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("b") == 0u);

        REQUIRE(res.m_JumpData.find("c") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("c") == 0u);

        res = dlx::Parser::Parse("a: b: c: NOP");
        CHECK_FALSE(res.m_Instructions.empty());
        REQUIRE(res.m_ParseErrors.empty());
        REQUIRE(res.m_JumpData.size() == 3);

        REQUIRE(res.m_JumpData.find("a") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("a") == 0u);

        REQUIRE(res.m_JumpData.find("b") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("b") == 0u);

        REQUIRE(res.m_JumpData.find("c") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("c") == 0u);

        res = dlx::Parser::Parse("a: ADD R1 R1 R1\nb: ADD R1 R1 R2\nc:ADD R1 R2 R3");
        REQUIRE(res.m_ParseErrors.empty());
        CHECK(res.m_Instructions.size() == 3);
        REQUIRE(res.m_JumpData.size() == 3);

        REQUIRE(res.m_JumpData.find("a") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("a") == 0u);

        REQUIRE(res.m_JumpData.find("b") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("b") == 1u);

        REQUIRE(res.m_JumpData.find("c") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("c") == 2u);
    }

    SECTION("Correct instructions")
    {
        SECTION("Is case insensitive")
        {
            res = dlx::Parser::Parse("add, r2, r4, r29");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADD,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));

            res = dlx::Parser::Parse("aDd, r2, r4, R29");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADD,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));

            res = dlx::Parser::Parse("AdD, r2, r4, R29");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADD,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));
        }

        // Arithmetic
        SECTION("ADD")
        {
            res = dlx::Parser::Parse("ADD, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADD,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ADDI")
        {
            res = dlx::Parser::Parse("ADDI, R1, R2, #25");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(25)));
        }

        SECTION("ADDU")
        {
            res = dlx::Parser::Parse("ADDU, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ADDUI")
        {
            res = dlx::Parser::Parse("ADDUI, R1, R2, #25");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(25)));
        }

        SECTION("ADDF")
        {
            res = dlx::Parser::Parse("ADDF F0 F1 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2)));
        }

        SECTION("ADDD")
        {
            res = dlx::Parser::Parse("ADDD F0 F2 F4");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F4)));
        }

        SECTION("SUB")
        {
            res = dlx::Parser::Parse("SUB, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUB,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SUBI")
        {
            res = dlx::Parser::Parse("SUBI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SUBU")
        {
            res = dlx::Parser::Parse("SUBU, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SUBUI")
        {
            res = dlx::Parser::Parse("SUBUI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SUBF")
        {
            res = dlx::Parser::Parse("SUBF F0 F1 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2)));
        }

        SECTION("SUBD")
        {
            res = dlx::Parser::Parse("SUBD F0 F2 F4");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F4)));
        }

        SECTION("MULT")
        {
            res = dlx::Parser::Parse("MULT, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULT,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("MULTI")
        {
            res = dlx::Parser::Parse("MULTI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("MULTU")
        {
            res = dlx::Parser::Parse("MULTU, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("MULTUI")
        {
            res = dlx::Parser::Parse("MULTUI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("MULTF")
        {
            res = dlx::Parser::Parse("MULTF F0 F1 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2)));
        }

        SECTION("MULTD")
        {
            res = dlx::Parser::Parse("MULTD F0 F2 F4");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F4)));
        }

        SECTION("DIV")
        {
            res = dlx::Parser::Parse("DIV, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIV,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("DIVI")
        {
            res = dlx::Parser::Parse("DIVI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("DIVU")
        {
            res = dlx::Parser::Parse("DIVU, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("DIVUI")
        {
            res = dlx::Parser::Parse("DIVUI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("DIVF")
        {
            res = dlx::Parser::Parse("DIVF F0 F1 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2)));
        }

        SECTION("DIVD")
        {
            res = dlx::Parser::Parse("DIVD F0 F2 F4");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F4)));
        }

        SECTION("SLL")
        {
            res = dlx::Parser::Parse("SLL, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLL,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLLI")
        {
            res = dlx::Parser::Parse("SLLI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLLI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SRL")
        {
            res = dlx::Parser::Parse("SRL, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SRL,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SRLI")
        {
            res = dlx::Parser::Parse("SRLI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SRLI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SLA")
        {
            res = dlx::Parser::Parse("SLA, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLA,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLAI")
        {
            res = dlx::Parser::Parse("SLAI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLAI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SRA")
        {
            res = dlx::Parser::Parse("SRA, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SRA,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SRAI")
        {
            res = dlx::Parser::Parse("SRAI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SRAI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        // Logic
        SECTION("AND")
        {
            res = dlx::Parser::Parse("AND, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::AND,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ANDI")
        {
            res = dlx::Parser::Parse("ANDI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ANDI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("OR")
        {
            res = dlx::Parser::Parse("OR, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::OR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ORI")
        {
            res = dlx::Parser::Parse("ORI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ORI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("XOR")
        {
            res = dlx::Parser::Parse("XOR, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::XOR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("XORI")
        {
            res = dlx::Parser::Parse("XORI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::XORI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        // Set conditionals
        SECTION("SLT")
        {
            res = dlx::Parser::Parse("SLT, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLT,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLTI")
        {
            res = dlx::Parser::Parse("SLTI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLTI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SLTU")
        {
            res = dlx::Parser::Parse("SLTU R1 R2 R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLTU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLTUI")
        {
            res = dlx::Parser::Parse("SLTUI R1 R2 #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLTUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("LTF")
        {
            res = dlx::Parser::Parse("LTF F0 F1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LTF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::InstructionArg()));
        }

        SECTION("LTD")
        {
            res = dlx::Parser::Parse("LTF F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LTF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("SGT")
        {
            res = dlx::Parser::Parse("SGT, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SGT,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SGTI")
        {
            res = dlx::Parser::Parse("SGTI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SGTI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SGTU")
        {
            res = dlx::Parser::Parse("SGTU R1 R2 R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SGTU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SGTUI")
        {
            res = dlx::Parser::Parse("SGTUI R1 R2 #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SGTUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("GTF")
        {
            res = dlx::Parser::Parse("GTF F0 F1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::GTF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::InstructionArg()));
        }

        SECTION("GTD")
        {
            res = dlx::Parser::Parse("GTD F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::GTD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("SLE")
        {
            res = dlx::Parser::Parse("SLE, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLE,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLEI")
        {
            res = dlx::Parser::Parse("SLEI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLEI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SLEU")
        {
            res = dlx::Parser::Parse("SLEU R1 R2 R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLEU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLEUI")
        {
            res = dlx::Parser::Parse("SLEUI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLEUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("LEF")
        {
            res = dlx::Parser::Parse("LEF F0 F1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LEF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::InstructionArg()));
        }

        SECTION("LED")
        {
            res = dlx::Parser::Parse("LED F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LED,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("SEQ")
        {
            res = dlx::Parser::Parse("SEQ, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SEQ,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SEQI")
        {
            res = dlx::Parser::Parse("SEQI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SEQI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SEQU")
        {
            res = dlx::Parser::Parse("SEQU R1 R2 R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SEQU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SEQUI")
        {
            res = dlx::Parser::Parse("SEQUI R1 R2 #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SEQUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("EQF")
        {
            res = dlx::Parser::Parse("EQF F0 F1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::EQF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::InstructionArg()));
        }

        SECTION("EQD")
        {
            res = dlx::Parser::Parse("EQD F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::EQD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("SNE")
        {
            res = dlx::Parser::Parse("SNE, R1, R2, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SNE,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SNEI")
        {
            res = dlx::Parser::Parse("SNEI, R1, R2, #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SNEI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SNEU")
        {
            res = dlx::Parser::Parse("SNEU R1 R2 R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SNEU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SNEUI")
        {
            res = dlx::Parser::Parse("SNEUI R1 R2 #21");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SNEUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("NEF")
        {
            res = dlx::Parser::Parse("NEF F0 F1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::NEF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::InstructionArg()));
        }

        SECTION("NED")
        {
            res = dlx::Parser::Parse("NED F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::NED,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        // Conditional branching
        SECTION("BEQZ")
        {
            res = dlx::Parser::Parse("BEQZ, R5, jump_label");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::BEQZ,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R5),
                    dlx::ConstructInstructionArgLabel("jump_label"), dlx::InstructionArg()));
        }

        SECTION("BNEZ")
        {
            res = dlx::Parser::Parse("BNEZ, R5, jump_label");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::BNEZ,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R5),
                    dlx::ConstructInstructionArgLabel("jump_label"), dlx::InstructionArg()));
        }

        SECTION("BFPT")
        {
            res = dlx::Parser::Parse("BFPT jump_label");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::BFPT,
                                     dlx::ConstructInstructionArgLabel("jump_label"),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("BFPF")
        {
            res = dlx::Parser::Parse("BFPF jump_label");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::BFPF,
                                     dlx::ConstructInstructionArgLabel("jump_label"),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        // Unconditional branching
        SECTION("J")
        {
            res = dlx::Parser::Parse("J, jump_label");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::J,
                                     dlx::ConstructInstructionArgLabel("jump_label"),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("JR")
        {
            res = dlx::Parser::Parse("JR, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::JR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3),
                    dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("JAL")
        {
            res = dlx::Parser::Parse("JAL, jump_label");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::JAL,
                                     dlx::ConstructInstructionArgLabel("jump_label"),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("JALR")
        {
            res = dlx::Parser::Parse("JALR, R3");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::JALR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3),
                    dlx::InstructionArg(), dlx::InstructionArg()));
        }

        // Loading data
        SECTION("LHI")
        {
            res = dlx::Parser::Parse("LHI R21 #1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LHI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1), dlx::InstructionArg()));
        }

        SECTION("LB")
        {
            res = dlx::Parser::Parse("LB, R21, #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LB,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LB, R21, 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LB,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LBU")
        {
            res = dlx::Parser::Parse("LBU, R21, #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LBU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LBU, R21, 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LBU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LH")
        {
            res = dlx::Parser::Parse("LH, R21, #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LH,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LH, R21, 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LH,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LHU")
        {
            res = dlx::Parser::Parse("LHU, R21, #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LHU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LHU, R21, 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LHU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LW")
        {
            res = dlx::Parser::Parse("LW, R21, #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LW,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LW, R21, 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LW,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LWU")
        {
            res = dlx::Parser::Parse("LWU, R21, #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LWU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LWU, R21, 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LWU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LF")
        {
            res = dlx::Parser::Parse("LF F0 #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LF F0 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LD")
        {
            res = dlx::Parser::Parse("LD F0 #1000");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse("LD F0 1000(R0)");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        // Store data
        SECTION("SB")
        {
            res = dlx::Parser::Parse("SB, #1000, R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SB,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SB, 1000(R0), R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SB,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SBU")
        {
            res = dlx::Parser::Parse("SBU, #1000, R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SBU,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SBU, 1000(R0), R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SBU,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SH")
        {
            res = dlx::Parser::Parse("SH, #1000, R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SH,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SH, 1000(R0), R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SH,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SHU")
        {
            res = dlx::Parser::Parse("SHU, #1000, R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SHU,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SHU, 1000(R0), R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SHU,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SW")
        {
            res = dlx::Parser::Parse("SW, #1000, R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SW,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SW, 1000(R0), R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SW,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SWU")
        {
            res = dlx::Parser::Parse("SWU, #1000, R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SWU,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SWU, 1000(R0), R12");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SWU,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SF")
        {
            res = dlx::Parser::Parse("SF #1000 F0");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SF,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SF 1000(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SF,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::InstructionArg()));
        }

        SECTION("SD")
        {
            res = dlx::Parser::Parse("SD #1000 F0");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SD,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse("SD 1000(R0) F0");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SD,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::InstructionArg()));
        }

        // Moving data
        SECTION("MOVF")
        {
            res = dlx::Parser::Parse("MOVF F0 F1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MOVF,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::InstructionArg()));
        }

        SECTION("MOVD")
        {
            res = dlx::Parser::Parse("MOVD F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MOVD,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("MOVFP2I")
        {
            res = dlx::Parser::Parse("MOVFP2I R1 F1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MOVFP2I,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::InstructionArg()));
        }

        SECTION("MOVI2FP")
        {
            res = dlx::Parser::Parse("MOVI2FP F1 R1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MOVI2FP,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::InstructionArg()));
        }

        // Converting data
        SECTION("CVTF2D")
        {
            res = dlx::Parser::Parse("CVTF2D F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::CVTF2D,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("CVTF2I")
        {
            res = dlx::Parser::Parse("CVTF2I F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::CVTF2I,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("CVTD2F")
        {
            res = dlx::Parser::Parse("CVTD2F F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::CVTD2F,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("CVTF2I")
        {
            res = dlx::Parser::Parse("CVTF2I F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::CVTF2I,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("CVTI2F")
        {
            res = dlx::Parser::Parse("CVTI2F F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::CVTI2F,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        SECTION("CVTI2D")
        {
            res = dlx::Parser::Parse("CVTI2D F0 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::CVTI2D,
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0),
                    dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2),
                    dlx::InstructionArg()));
        }

        // Special
        SECTION("TRAP")
        {
            res = dlx::Parser::Parse("TRAP #1");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::TRAP,
                                     dlx::ConstructInstructionArgImmediateValue(1),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("HALT")
        {
            res = dlx::Parser::Parse("HALT");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::HALT,
                                     dlx::InstructionArg(), dlx::InstructionArg(),
                                     dlx::InstructionArg()));
        }

        SECTION("NOP")
        {
            res = dlx::Parser::Parse("NOP");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::NOP,
                                     dlx::InstructionArg(), dlx::InstructionArg(),
                                     dlx::InstructionArg()));
        }
    }

    SECTION("Not parsing private members")
    {
        SECTION("OpCode::NONE")
        {
            res = dlx::Parser::Parse("NONE");
            CHECK(res.m_Instructions.empty());
            CHECK_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("OpCode::NUMBER_OF_ELEMENTS")
        {
            res = dlx::Parser::Parse("NUMBER_OF_ELEMENTS");
            CHECK(res.m_Instructions.empty());
            CHECK_FALSE(res.m_ParseErrors.empty());
        }
    }

    SECTION("Parsing errors")
    {
        SECTION("Missing arguments")
        {
            res = dlx::Parser::Parse("J");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J\n");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J :");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J #");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J -");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J .");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J label:");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADD");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("SW");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Require IntRegister")
        {
            res = dlx::Parser::Parse("ADD R1 R1 #25");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADD R1 R1 25(R0)");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADD R1 R1 label");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADD R1 R1 F0");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADD R1 R1 FPSR");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADD R1 R1 ADD");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Require immediate value")
        {
            res = dlx::Parser::Parse("ADDI R1 R1 R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADDI R1 R1 25(R0)");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADDI R1 R1 label");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADDI R1 R1 F0");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADDI R1 R1 FPSR");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADDI R1 R1 ADD");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Require label")
        {
            res = dlx::Parser::Parse("J R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J 25(R0)");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J #25");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J F0");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J FPSR");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("J ADD");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Address displacement or immediate value")
        {
            res = dlx::Parser::Parse("LW R1 label");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 R2");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 F0");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 FPSR");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 ADD");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Incomplete Address displacement")
        {
            res = dlx::Parser::Parse("LW R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 999999999");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000(");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000:");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000(F1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000(FPSR");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000()");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000(R1:");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000(R1 F1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("LW R1 1000(R1 / Comment");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Immediate integer value too large")
        {
            res = dlx::Parser::Parse("ADDI R1 R0 #999999999999");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("ADDI R1 R0 #-999999999999");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Unexpected label")
        {
            res = dlx::Parser::Parse("j: ADD R1 R1 R1 j");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("j: ADD R1 R1 R1 l:");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Invalid label names")
        {
            res = dlx::Parser::Parse("add: ADD R1 R1 R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("R1: ADD R1 R1 R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("F1: ADD R1 R1 R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("FPSR: ADD R1 R1 R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("No empty labels")
        {
            res = dlx::Parser::Parse("label:");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a:\nb:");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a:\n");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a:\n//Comment");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Label names need to unique")
        {
            res = dlx::Parser::Parse("l:\nl:\nNOP");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a:\nNOP\nb:NOP\na:NOP");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a:NOP\na:NOP");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a:a:NOP");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a: a:NOP");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse("a: a: a: a:\nNOP");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }
    }
}
