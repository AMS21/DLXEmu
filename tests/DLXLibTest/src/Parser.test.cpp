#include <catch2/catch_test_macros.hpp>

#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <Phi/Core/Log.hpp>

phi::Boolean TokenMatches(const dlx::Token& token, const std::string& text, dlx::Token::Type type)
{
    return token.GetText() == text && token.GetType() == type;
}

TEST_CASE("Parser tokenize")
{
    //phi::Log::initialize_default_loggers();

    std::vector<dlx::Token> res;
    res.reserve(30);

    SECTION("Ignoring whitespaces")
    {
        res = dlx::Parser::Tokenize("    ");
        CHECK(res.empty());

        res = dlx::Parser::Tokenize("\t  \v\tADD\t  \v");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), "ADD", dlx::Token::Type::Identifier));
    }

    SECTION("Newlines")
    {
        res = dlx::Parser::Tokenize("\n\n");
        REQUIRE(res.size() == 2);
        CHECK(TokenMatches(res.at(0), "\n", dlx::Token::Type::NewLine));
        CHECK(TokenMatches(res.at(1), "\n", dlx::Token::Type::NewLine));

        res = dlx::Parser::Tokenize("ADD\n");
        REQUIRE(res.size() == 2);
        CHECK(TokenMatches(res.at(0), "ADD", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(1), "\n", dlx::Token::Type::NewLine));
    }

    SECTION("Comments")
    {
        res = dlx::Parser::Tokenize("/ This is a comment");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), "/ This is a comment", dlx::Token::Type::Comment));

        res = dlx::Parser::Tokenize("; Also a comment");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), "; Also a comment", dlx::Token::Type::Comment));

        res = dlx::Parser::Tokenize("ADD ; Trailing comments are nice");
        REQUIRE(res.size() == 2);
        CHECK(TokenMatches(res.at(0), "ADD", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(1), "; Trailing comments are nice", dlx::Token::Type::Comment));

        res = dlx::Parser::Tokenize(";One line comment\n;Next line comment\nADD");
        REQUIRE(res.size() == 5);
        CHECK(TokenMatches(res.at(0), ";One line comment", dlx::Token::Type::Comment));
        CHECK(TokenMatches(res.at(1), "\n", dlx::Token::Type::NewLine));
        CHECK(TokenMatches(res.at(2), ";Next line comment", dlx::Token::Type::Comment));
        CHECK(TokenMatches(res.at(3), "\n", dlx::Token::Type::NewLine));
        CHECK(TokenMatches(res.at(4), "ADD", dlx::Token::Type::Identifier));

        res = dlx::Parser::Tokenize("; Comment: With (s)peci(a)l chars, /// \\ ;;");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), "; Comment: With (s)peci(a)l chars, /// \\ ;;",
                           dlx::Token::Type::Comment));
    }

    SECTION("Colon")
    {
        res = dlx::Parser::Tokenize(":");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), ":", dlx::Token::Type::Colon));

        res = dlx::Parser::Tokenize("start:");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), "start:", dlx::Token::Type::Identifier));
    }

    SECTION("Comma")
    {
        res = dlx::Parser::Tokenize(",");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), ",", dlx::Token::Type::Comma));

        res = dlx::Parser::Tokenize("ADD, R1, R3, R2");
        REQUIRE(res.size() == 7);
        CHECK(TokenMatches(res.at(0), "ADD", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(1), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.at(2), "R1", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(3), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.at(4), "R3", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(5), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.at(6), "R2", dlx::Token::Type::Identifier));

        res = dlx::Parser::Tokenize("ADD,R1,R3,R2");
        REQUIRE(res.size() == 7);
        CHECK(TokenMatches(res.at(0), "ADD", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(1), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.at(2), "R1", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(3), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.at(4), "R3", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(5), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.at(6), "R2", dlx::Token::Type::Identifier));
    }

    SECTION("Brackets")
    {
        res = dlx::Parser::Tokenize("(");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), "(", dlx::Token::Type::OpenBracket));

        res = dlx::Parser::Tokenize(")");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), ")", dlx::Token::Type::ClosingBracket));

        res = dlx::Parser::Tokenize("1000(R3)");
        REQUIRE(res.size() == 4);
        CHECK(TokenMatches(res.at(0), "1000", dlx::Token::Type::IntegerLiteral));
        CHECK(TokenMatches(res.at(1), "(", dlx::Token::Type::OpenBracket));
        CHECK(TokenMatches(res.at(2), "R3", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(3), ")", dlx::Token::Type::ClosingBracket));
    }

    SECTION("identifier")
    {
        res = dlx::Parser::Tokenize("identifier");
        REQUIRE(res.size() == 1);
        CHECK(TokenMatches(res.at(0), "identifier", dlx::Token::Type::Identifier));

        res = dlx::Parser::Tokenize("ADD R0 R12 R31");
        REQUIRE(res.size() == 4);
        CHECK(TokenMatches(res.at(0), "ADD", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(1), "R0", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(2), "R12", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(3), "R31", dlx::Token::Type::Identifier));

        res = dlx::Parser::Tokenize("J label");
        REQUIRE(res.size() == 2);
        CHECK(TokenMatches(res.at(0), "J", dlx::Token::Type::Identifier));
        CHECK(TokenMatches(res.at(1), "label", dlx::Token::Type::Identifier));
    }
}

TEST_CASE("Only one instruction per line")
{
    dlx::ParsedProgram      res;
    dlx::InstructionLibrary lib;

    res = dlx::Parser::Parse(lib, "ADD R1 R2 R3 ADD R1 R2 R3");
    CHECK(!res.m_ParseErrors.empty());
}

phi::Boolean InstructionMatches(const dlx::Instruction& instr, dlx::OpCode opcode,
                                dlx::InstructionArg arg1, dlx::InstructionArg arg2,
                                dlx::InstructionArg arg3)
{
    if (instr.m_Info.GetOpCode() != opcode)
    {
        return false;
    }

    if (instr.m_Arg1 != arg1)
    {
        return false;
    }

    if (instr.m_Arg2 != arg2)
    {
        return false;
    }

    if (instr.m_Arg3 != arg3)
    {
        return false;
    }

    return true;
}

TEST_CASE("Parser")
{
    //phi::Log::initialize_default_loggers();

    dlx::ParsedProgram      res;
    dlx::InstructionLibrary lib;

    SECTION("Ignoring comments")
    {
        res = dlx::Parser::Parse(lib, "; Comment");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());

        res = dlx::Parser::Parse(lib, "/Comment");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());

        res = dlx::Parser::Parse(lib, "// First comment\n;Second comment");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
    }

    SECTION("Ignoring new lines")
    {
        res = dlx::Parser::Parse(lib, "\n");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());

        res = dlx::Parser::Parse(lib, "\n\n\n\n");
        CHECK(res.m_Instructions.empty());
        CHECK(res.m_JumpData.empty());
    }

    SECTION("Jump labels")
    {
        res = dlx::Parser::Parse(lib, "start:");
        CHECK(res.m_Instructions.empty());
        REQUIRE(res.m_ParseErrors.empty());

        REQUIRE(res.m_JumpData.size() == 1);
        REQUIRE(res.m_JumpData.find("start") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("start") == 0u);

        res = dlx::Parser::Parse(lib, "a:\nb:\nc:");
        CHECK(res.m_Instructions.empty());
        REQUIRE(res.m_ParseErrors.empty());
        REQUIRE(res.m_JumpData.size() == 3);

        REQUIRE(res.m_JumpData.find("a") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("a") == 0u);

        REQUIRE(res.m_JumpData.find("b") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("b") == 0u);

        REQUIRE(res.m_JumpData.find("c") != res.m_JumpData.end());
        CHECK(res.m_JumpData.at("c") == 0u);

        res = dlx::Parser::Parse(lib, "a: ADD R1 R1 R1\nb: ADD R1 R1 R2\nc:ADD R1 R2 R3");
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
            res = dlx::Parser::Parse(lib, "add, r2, r4, r29");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADD,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));

            res = dlx::Parser::Parse(lib, "aDd, r2, r4, R29");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADD,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R4),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R29)));

            res = dlx::Parser::Parse(lib, "AdD, r2, r4, R29");
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
            res = dlx::Parser::Parse(lib, "ADD, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADD,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ADDI")
        {
            res = dlx::Parser::Parse(lib, "ADDI, R1, R2, #25");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(25)));
        }

        SECTION("ADDU")
        {
            res = dlx::Parser::Parse(lib, "ADDU, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ADDUI")
        {
            res = dlx::Parser::Parse(lib, "ADDUI, R1, R2, #25");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ADDUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(25)));
        }

        SECTION("SUB")
        {
            res = dlx::Parser::Parse(lib, "SUB, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUB,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SUBI")
        {
            res = dlx::Parser::Parse(lib, "SUBI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SUBU")
        {
            res = dlx::Parser::Parse(lib, "SUBU, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SUBUI")
        {
            res = dlx::Parser::Parse(lib, "SUBUI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SUBUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("MULT")
        {
            res = dlx::Parser::Parse(lib, "MULT, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULT,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("MULTI")
        {
            res = dlx::Parser::Parse(lib, "MULTI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("MULTU")
        {
            res = dlx::Parser::Parse(lib, "MULTU, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("MULTUI")
        {
            res = dlx::Parser::Parse(lib, "MULTUI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::MULTUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("DIV")
        {
            res = dlx::Parser::Parse(lib, "DIV, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIV,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("DIVI")
        {
            res = dlx::Parser::Parse(lib, "DIVI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("DIVU")
        {
            res = dlx::Parser::Parse(lib, "DIVU, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("DIVUI")
        {
            res = dlx::Parser::Parse(lib, "DIVUI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::DIVUI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SLL")
        {
            res = dlx::Parser::Parse(lib, "SLL, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLL,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLLI")
        {
            res = dlx::Parser::Parse(lib, "SLLI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLLI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SRL")
        {
            res = dlx::Parser::Parse(lib, "SRL, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SRL,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SRLI")
        {
            res = dlx::Parser::Parse(lib, "SRLI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SRLI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SLA")
        {
            res = dlx::Parser::Parse(lib, "SLA, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLA,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLAI")
        {
            res = dlx::Parser::Parse(lib, "SLAI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLAI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SRA")
        {
            res = dlx::Parser::Parse(lib, "SRA, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SRA,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SRAI")
        {
            res = dlx::Parser::Parse(lib, "SRAI, R1, R2, #21");
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
            res = dlx::Parser::Parse(lib, "AND, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::AND,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ANDI")
        {
            res = dlx::Parser::Parse(lib, "ANDI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ANDI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("OR")
        {
            res = dlx::Parser::Parse(lib, "OR, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::OR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("ORI")
        {
            res = dlx::Parser::Parse(lib, "ORI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::ORI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("XOR")
        {
            res = dlx::Parser::Parse(lib, "XOR, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::XOR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("XORI")
        {
            res = dlx::Parser::Parse(lib, "XORI, R1, R2, #21");
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
            res = dlx::Parser::Parse(lib, "SLT, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLT,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLTI")
        {
            res = dlx::Parser::Parse(lib, "SLTI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLTI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SGT")
        {
            res = dlx::Parser::Parse(lib, "SGT, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SGT,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SGTI")
        {
            res = dlx::Parser::Parse(lib, "SGTI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SGTI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SLE")
        {
            res = dlx::Parser::Parse(lib, "SLE, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLE,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SLEI")
        {
            res = dlx::Parser::Parse(lib, "SLEI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SLEI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SEQ")
        {
            res = dlx::Parser::Parse(lib, "SEQ, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SEQ,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SEQI")
        {
            res = dlx::Parser::Parse(lib, "SEQI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SEQI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        SECTION("SNE")
        {
            res = dlx::Parser::Parse(lib, "SNE, R1, R2, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SNE,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3)));
        }

        SECTION("SNEI")
        {
            res = dlx::Parser::Parse(lib, "SNEI, R1, R2, #21");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SNEI,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R1),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R2),
                    dlx::ConstructInstructionArgImmediateValue(21)));
        }

        // Conditional branching
        SECTION("BEQZ")
        {
            res = dlx::Parser::Parse(lib, "BEQZ, R5, jump_label");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::BEQZ,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R5),
                    dlx::ConstructInstructionArgLabel("jump_label"), dlx::InstructionArg()));
        }

        SECTION("BNEZ")
        {
            res = dlx::Parser::Parse(lib, "BNEZ, R5, jump_label");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::BNEZ,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R5),
                    dlx::ConstructInstructionArgLabel("jump_label"), dlx::InstructionArg()));
        }

        // Unconditional branching
        SECTION("J")
        {
            res = dlx::Parser::Parse(lib, "J, jump_label");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::J,
                                     dlx::ConstructInstructionArgLabel("jump_label"),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("JR")
        {
            res = dlx::Parser::Parse(lib, "JR, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::JR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3),
                    dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("JAL")
        {
            res = dlx::Parser::Parse(lib, "JAL, jump_label");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::JAL,
                                     dlx::ConstructInstructionArgLabel("jump_label"),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("JALR")
        {
            res = dlx::Parser::Parse(lib, "JALR, R3");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::JALR,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R3),
                    dlx::InstructionArg(), dlx::InstructionArg()));
        }

        // Loading data
        SECTION("LB")
        {
            res = dlx::Parser::Parse(lib, "LB, R21, #1000");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LB,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "LB, R21, 1000(R0)");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LB,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LBU")
        {
            res = dlx::Parser::Parse(lib, "LBU, R21, #1000");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LBU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "LBU, R21, 1000(R0)");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LBU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LH")
        {
            res = dlx::Parser::Parse(lib, "LH, R21, #1000");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LH,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "LH, R21, 1000(R0)");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LH,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LHU")
        {
            res = dlx::Parser::Parse(lib, "LHU, R21, #1000");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LHU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "LHU, R21, 1000(R0)");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LHU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LW")
        {
            res = dlx::Parser::Parse(lib, "LW, R21, #1000");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LW,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "LW, R21, 1000(R0)");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LW,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        SECTION("LWU")
        {
            res = dlx::Parser::Parse(lib, "LWU, R21, #1000");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LWU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgImmediateValue(1000), dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "LWU, R21, 1000(R0)");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::LWU,
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R21),
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::InstructionArg()));
        }

        // Store data
        SECTION("SB")
        {
            res = dlx::Parser::Parse(lib, "SB, #1000, R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SB,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "SB, 1000(R0), R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SB,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SBU")
        {
            res = dlx::Parser::Parse(lib, "SBU, #1000, R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SBU,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "SBU, 1000(R0), R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SBU,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SH")
        {
            res = dlx::Parser::Parse(lib, "SH, #1000, R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SH,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "SH, 1000(R0), R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SH,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SHU")
        {
            res = dlx::Parser::Parse(lib, "SHU, #1000, R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SHU,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "SHU, 1000(R0), R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SHU,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SW")
        {
            res = dlx::Parser::Parse(lib, "SW, #1000, R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SW,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "SW, 1000(R0), R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SW,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        SECTION("SWU")
        {
            res = dlx::Parser::Parse(lib, "SWU, #1000, R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SWU,
                    dlx::ConstructInstructionArgImmediateValue(1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));

            res = dlx::Parser::Parse(lib, "SWU, 1000(R0), R12");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(
                    res.m_Instructions.at(0), dlx::OpCode::SWU,
                    dlx::ConstructInstructionArgAddressDisplacement(dlx::IntRegisterID::R0, 1000),
                    dlx::ConstructInstructionArgRegisterInt(dlx::IntRegisterID::R12),
                    dlx::InstructionArg()));
        }

        // Floating Point
        SECTION("ADDF")
        {
            res = dlx::Parser::Parse(lib, "ADDF F0 F1 F2");
            REQUIRE(res.m_ParseErrors.empty());
            REQUIRE(res.m_Instructions.size() == 1);

            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::ADDF, dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F0), dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F1), dlx::ConstructInstructionArgRegisterFloat(dlx::FloatRegisterID::F2)));
        }

        // Other
        SECTION("TRAP")
        {
            res = dlx::Parser::Parse(lib, "TRAP #1");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::TRAP,
                                     dlx::ConstructInstructionArgImmediateValue(1),
                                     dlx::InstructionArg(), dlx::InstructionArg()));
        }

        SECTION("HALT")
        {
            res = dlx::Parser::Parse(lib, "HALT");
            REQUIRE(res.m_Instructions.size() == 1);
            CHECK(InstructionMatches(res.m_Instructions.at(0), dlx::OpCode::HALT,
                                     dlx::InstructionArg(), dlx::InstructionArg(),
                                     dlx::InstructionArg()));
        }

        SECTION("NOP")
        {
            res = dlx::Parser::Parse(lib, "NOP");
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
            res = dlx::Parser::Parse(lib, "NONE");
            CHECK(res.m_Instructions.empty());
            CHECK_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("OpCode::NUMBER_OF_ELEMENTS")
        {
            res = dlx::Parser::Parse(lib, "NUMBER_OF_ELEMENTS");
            CHECK(res.m_Instructions.empty());
            CHECK_FALSE(res.m_ParseErrors.empty());
        }
    }

    SECTION("Parsing errors")
    {
        SECTION("Missing arguments")
        {
            res = dlx::Parser::Parse(lib, "J");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J\n");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J :");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J #");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J -");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J .");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J label:");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Require IntRegister")
        {
            res = dlx::Parser::Parse(lib, "ADD R1 R1 #25");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "ADD R1 R1 25(R0)");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "ADD R1 R1 label");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Require immediate value")
        {
            res = dlx::Parser::Parse(lib, "ADDI R1 R1 R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "ADDI R1 R1 25(R0)");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "ADDI R1 R1 label");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Require label")
        {
            res = dlx::Parser::Parse(lib, "J R1");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J 25(R0)");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "J #25");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }

        SECTION("Address displacement or immediate value")
        {
            res = dlx::Parser::Parse(lib, "LW R1 label");
            REQUIRE_FALSE(res.m_ParseErrors.empty());

            res = dlx::Parser::Parse(lib, "LW R1 R2");
            REQUIRE_FALSE(res.m_ParseErrors.empty());
        }
    }
}
