#include "DLX/TokenStream.hpp"
#include <catch2/catch_test_macros.hpp>

#include <DLX/Tokenize.hpp>

phi::Boolean TokenMatches(const dlx::Token& token, const std::string& text, dlx::Token::Type type)
{
    return token.GetText() == text && token.GetType() == type;
}

TEST_CASE("Parser tokenize")
{
    dlx::TokenStream res;

    SECTION("Ignoring whitespaces")
    {
        res = dlx::Tokenize("    ");
        CHECK(res.reached_end());

        res = dlx::Tokenize("\t  \v\tADD\t  \v");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
    }

    SECTION("Newlines")
    {
        res = dlx::Tokenize("\n\n");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine));
        CHECK(TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine));

        res = dlx::Tokenize("ADD\n");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine));
    }

    SECTION("Comments")
    {
        res = dlx::Tokenize("/ This is a comment");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "/ This is a comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("; Also a comment");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "; Also a comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("ADD ; Trailing comments are nice");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), "; Trailing comments are nice",
                           dlx::Token::Type::Comment));

        res = dlx::Tokenize(";One line comment\n;Next line comment\nADD");
        REQUIRE(bool(res.size() == 5u));
        CHECK(TokenMatches(res.consume(), ";One line comment", dlx::Token::Type::Comment));
        CHECK(TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine));
        CHECK(TokenMatches(res.consume(), ";Next line comment", dlx::Token::Type::Comment));
        CHECK(TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));

        res = dlx::Tokenize("; Comment: With (s)peci(a)l chars, /// \\ ;;");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "; Comment: With (s)peci(a)l chars, /// \\ ;;",
                           dlx::Token::Type::Comment));

        // Comment characters seperate tokens
        res = dlx::Tokenize("ADD;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("ADD/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("R1;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("R1/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("F1;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "F1", dlx::Token::Type::RegisterFloat));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("F1/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "F1", dlx::Token::Type::RegisterFloat));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("FPSR;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "FPSR", dlx::Token::Type::RegisterStatus));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("FPSR/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "FPSR", dlx::Token::Type::RegisterStatus));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("label:;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "label:", dlx::Token::Type::LabelIdentifier));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("label:/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "label:", dlx::Token::Type::LabelIdentifier));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("10;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "10", dlx::Token::Type::IntegerLiteral));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("10/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "10", dlx::Token::Type::IntegerLiteral));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("#10;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "#10", dlx::Token::Type::ImmediateInteger));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("#10/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "#10", dlx::Token::Type::ImmediateInteger));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("(;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("(/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize(");Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize(")/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize(":;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), ":", dlx::Token::Type::Colon));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize(":/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), ":", dlx::Token::Type::Colon));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize(",;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize(",/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("identifier;Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "identifier", dlx::Token::Type::LabelIdentifier));
        CHECK(TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment));

        res = dlx::Tokenize("identifier/Comment");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "identifier", dlx::Token::Type::LabelIdentifier));
        CHECK(TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment));
    }

    SECTION("Colon")
    {
        res = dlx::Tokenize(":");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), ":", dlx::Token::Type::Colon));

        res = dlx::Tokenize("start:");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "start:", dlx::Token::Type::LabelIdentifier));
    }

    SECTION("Comma")
    {
        res = dlx::Tokenize(",");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));

        res = dlx::Tokenize("ADD, R1, R3, R2");
        REQUIRE(bool(res.size() == 7u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), "R3", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), "R2", dlx::Token::Type::RegisterInt));

        res = dlx::Tokenize("ADD,R1,R3,R2");
        REQUIRE(bool(res.size() == 7u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), "R3", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), ",", dlx::Token::Type::Comma));
        CHECK(TokenMatches(res.consume(), "R2", dlx::Token::Type::RegisterInt));
    }

    SECTION("Brackets")
    {
        res = dlx::Tokenize("(");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket));

        res = dlx::Tokenize(")");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket));

        res = dlx::Tokenize("1000(R3)");
        REQUIRE(bool(res.size() == 4u));
        CHECK(TokenMatches(res.consume(), "1000", dlx::Token::Type::IntegerLiteral));
        CHECK(TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket));
        CHECK(TokenMatches(res.consume(), "R3", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket));
    }

    SECTION("identifier")
    {
        res = dlx::Tokenize("identifier");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "identifier", dlx::Token::Type::LabelIdentifier));

        res = dlx::Tokenize("ADD R0 R12 R31");
        REQUIRE(bool(res.size() == 4u));
        CHECK(TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), "R0", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), "R12", dlx::Token::Type::RegisterInt));
        CHECK(TokenMatches(res.consume(), "R31", dlx::Token::Type::RegisterInt));

        res = dlx::Tokenize("J label");
        REQUIRE(bool(res.size() == 2u));
        CHECK(TokenMatches(res.consume(), "J", dlx::Token::Type::OpCode));
        CHECK(TokenMatches(res.consume(), "label", dlx::Token::Type::LabelIdentifier));

        res = dlx::Tokenize("_1");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "_1", dlx::Token::Type::LabelIdentifier));

        res = dlx::Tokenize("FPSR");
        REQUIRE(bool(res.size() == 1u));
        CHECK(TokenMatches(res.consume(), "FPSR", dlx::Token::Type::RegisterStatus));
    }
}
