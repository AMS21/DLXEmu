#include <phi/test/test_macros.hpp>

#include <DLX/EnumName.hpp>
#include <DLX/OpCode.hpp>
#include <DLX/Token.hpp>
#include <DLX/TokenStream.hpp>
#include <DLX/Tokenize.hpp>

void TokenMatches(const dlx::Token& token, const std::string_view expected_text,
                  dlx::Token::Type expected_type, std::size_t expected_line_number,
                  std::size_t expected_column)
{
    CHECK(token.GetText() == expected_text);
    CHECK(token.GetType() == expected_type);
    CHECK((token.GetLength() == expected_text.length()).unsafe());
    CHECK((token.GetLineNumber() == expected_line_number).unsafe());
    CHECK((token.GetColumn() == expected_column).unsafe());
}

TEST_CASE("Tokenize - Ignoring whitespaces")
{
    dlx::TokenStream res;

    res = dlx::Tokenize("");
    CHECK((res.size() == 0u).unsafe());
    CHECK(res.reached_end());

    res = dlx::Tokenize(" ");
    CHECK((res.size() == 0u).unsafe());
    CHECK(res.reached_end());

    res = dlx::Tokenize("\t");
    CHECK((res.size() == 0u).unsafe());
    CHECK(res.reached_end());

    res = dlx::Tokenize("\v");
    CHECK((res.size() == 0u).unsafe());
    CHECK(res.reached_end());

    res = dlx::Tokenize("    ");
    CHECK((res.size() == 0u).unsafe());
    CHECK(res.reached_end());

    res = dlx::Tokenize("\t  \v\tADD\t  \v");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1u, 6u);

    res = dlx::Tokenize("\t  \v\tADD\t  \vNOP");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1u, 6u);
    TokenMatches(res.consume(), "NOP", dlx::Token::Type::OpCode, 1u, 13u);
}

TEST_CASE("Tokenize - Newlines")
{
    dlx::TokenStream res;

    res = dlx::Tokenize("\n\n");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1, 1);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2, 1);

    res = dlx::Tokenize("ADD\n");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1, 4);

    res = dlx::Tokenize("\nADD\n");
    REQUIRE(bool(res.size() == 3u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1, 1);
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 2, 1);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2, 4);

    res = dlx::Tokenize(" \n");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);

    res = dlx::Tokenize("  \n");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 3u);

    res = dlx::Tokenize("   \n");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 4u);

    res = dlx::Tokenize("\t\n");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);

    res = dlx::Tokenize("\t\v\n");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 3u);

    res = dlx::Tokenize("\n\n\n");
    REQUIRE(bool(res.size() == 3u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 1u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);

    res = dlx::Tokenize("\n\n\nNOP");
    REQUIRE(bool(res.size() == 4u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 1u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);
    TokenMatches(res.consume(), "NOP", dlx::Token::Type::OpCode, 4u, 1u);

    res = dlx::Tokenize(" \n\n\nNOP");
    REQUIRE(bool(res.size() == 4u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);
    TokenMatches(res.consume(), "NOP", dlx::Token::Type::OpCode, 4u, 1u);

    res = dlx::Tokenize(" \n\n\n:");
    REQUIRE(bool(res.size() == 4u));
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);
    TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 4u, 1u);
}

TEST_CASE("Tokenize - Comments")
{
    dlx::TokenStream res;

    res = dlx::Tokenize("/ This is a comment");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "/ This is a comment", dlx::Token::Type::Comment, 1, 1);

    res = dlx::Tokenize("/This is a comment");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "/This is a comment", dlx::Token::Type::Comment, 1, 1);

    res = dlx::Tokenize("; Also a comment");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "; Also a comment", dlx::Token::Type::Comment, 1, 1);

    res = dlx::Tokenize(";Also a comment");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), ";Also a comment", dlx::Token::Type::Comment, 1, 1);

    res = dlx::Tokenize("ADD ; Trailing comments are nice");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), "; Trailing comments are nice", dlx::Token::Type::Comment, 1, 5);

    res = dlx::Tokenize(";One line comment\n;Next line comment\nADD");
    REQUIRE(bool(res.size() == 5u));
    TokenMatches(res.consume(), ";One line comment", dlx::Token::Type::Comment, 1, 1);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1, 18);
    TokenMatches(res.consume(), ";Next line comment", dlx::Token::Type::Comment, 2, 1);
    TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2, 19);
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 3, 1);

    res = dlx::Tokenize("; Comment: With (s)peci(a)l chars, /// \\ ;;");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "; Comment: With (s)peci(a)l chars, /// \\ ;;",
                 dlx::Token::Type::Comment, 1, 1);

    // Comment characters separate tokens
    res = dlx::Tokenize("ADD;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 4);

    res = dlx::Tokenize("ADD/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 4);

    res = dlx::Tokenize("R1;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 3);

    res = dlx::Tokenize("R1/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 3);

    res = dlx::Tokenize("F1;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "F1", dlx::Token::Type::RegisterFloat, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 3);

    res = dlx::Tokenize("F1/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "F1", dlx::Token::Type::RegisterFloat, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 3);

    res = dlx::Tokenize("FPSR;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "FPSR", dlx::Token::Type::RegisterStatus, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 5);

    res = dlx::Tokenize("FPSR/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "FPSR", dlx::Token::Type::RegisterStatus, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 5);

    res = dlx::Tokenize("label:;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "label:", dlx::Token::Type::LabelIdentifier, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 7);

    res = dlx::Tokenize("label:/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "label:", dlx::Token::Type::LabelIdentifier, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 7);

    res = dlx::Tokenize("10;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "10", dlx::Token::Type::IntegerLiteral, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 3);

    res = dlx::Tokenize("10/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "10", dlx::Token::Type::IntegerLiteral, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 3);

    res = dlx::Tokenize("#10;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "#10", dlx::Token::Type::ImmediateInteger, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 4);

    res = dlx::Tokenize("#10/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "#10", dlx::Token::Type::ImmediateInteger, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 4);

    res = dlx::Tokenize("(;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize("(/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize(");Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize(")/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize(":;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize(":/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize(",;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize(",/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 2);

    res = dlx::Tokenize("identifier;Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "identifier", dlx::Token::Type::LabelIdentifier, 1, 1);
    TokenMatches(res.consume(), ";Comment", dlx::Token::Type::Comment, 1, 11);

    res = dlx::Tokenize("identifier/Comment");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "identifier", dlx::Token::Type::LabelIdentifier, 1, 1);
    TokenMatches(res.consume(), "/Comment", dlx::Token::Type::Comment, 1, 11);

    res = dlx::Tokenize(":x;(");
    REQUIRE(bool(res.size() == 3u));
    TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 1u, 1u);
    TokenMatches(res.consume(), "x", dlx::Token::Type::LabelIdentifier, 1u, 2u);
    TokenMatches(res.consume(), ";(", dlx::Token::Type::Comment, 1u, 3u);
}

TEST_CASE("Tokenize - Colon")
{
    dlx::TokenStream res;

    res = dlx::Tokenize(":");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 1, 1);

    res = dlx::Tokenize("start:");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "start:", dlx::Token::Type::LabelIdentifier, 1, 1);

    res = dlx::Tokenize(":x");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 1, 1);
    TokenMatches(res.consume(), "x", dlx::Token::Type::LabelIdentifier, 1, 2);
}

TEST_CASE("Tokenize - Comma")
{
    dlx::TokenStream res;

    res = dlx::Tokenize(",");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 1);

    res = dlx::Tokenize("ADD, R1, R3, R2");
    REQUIRE(bool(res.size() == 7u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 4);
    TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt, 1, 6);
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 8);
    TokenMatches(res.consume(), "R3", dlx::Token::Type::RegisterInt, 1, 10);
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 12);
    TokenMatches(res.consume(), "R2", dlx::Token::Type::RegisterInt, 1, 14);

    res = dlx::Tokenize("ADD,R1,R3,R2");
    REQUIRE(bool(res.size() == 7u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 4);
    TokenMatches(res.consume(), "R1", dlx::Token::Type::RegisterInt, 1, 5);
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 7);
    TokenMatches(res.consume(), "R3", dlx::Token::Type::RegisterInt, 1, 8);
    TokenMatches(res.consume(), ",", dlx::Token::Type::Comma, 1, 10);
    TokenMatches(res.consume(), "R2", dlx::Token::Type::RegisterInt, 1, 11);
}

TEST_CASE("Tokenize - Brackets")
{
    dlx::TokenStream res;

    res = dlx::Tokenize("(");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket, 1, 1);

    res = dlx::Tokenize(")");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket, 1, 1);

    res = dlx::Tokenize("()");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket, 1, 1);
    TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket, 1, 2);

    res = dlx::Tokenize("1000(R3)");
    REQUIRE(bool(res.size() == 4u));
    TokenMatches(res.consume(), "1000", dlx::Token::Type::IntegerLiteral, 1, 1);
    TokenMatches(res.consume(), "(", dlx::Token::Type::OpenBracket, 1, 5);
    TokenMatches(res.consume(), "R3", dlx::Token::Type::RegisterInt, 1, 6);
    TokenMatches(res.consume(), ")", dlx::Token::Type::ClosingBracket, 1, 8);
}

TEST_CASE("Tokenize - identifier")
{
    dlx::TokenStream res;

    res = dlx::Tokenize("identifier");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "identifier", dlx::Token::Type::LabelIdentifier, 1, 1);

    res = dlx::Tokenize("ADD R0 R12 R31");
    REQUIRE(bool(res.size() == 4u));
    TokenMatches(res.consume(), "ADD", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), "R0", dlx::Token::Type::RegisterInt, 1, 5);
    TokenMatches(res.consume(), "R12", dlx::Token::Type::RegisterInt, 1, 8);
    TokenMatches(res.consume(), "R31", dlx::Token::Type::RegisterInt, 1, 12);

    res = dlx::Tokenize("J label");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "J", dlx::Token::Type::OpCode, 1, 1);
    TokenMatches(res.consume(), "label", dlx::Token::Type::LabelIdentifier, 1, 3);

    res = dlx::Tokenize("_1");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "_1", dlx::Token::Type::LabelIdentifier, 1, 1);

    res = dlx::Tokenize("FPSR");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "FPSR", dlx::Token::Type::RegisterStatus, 1, 1);
}

TEST_CASE("Tokenize -  ImmediateInteger")
{
    dlx::TokenStream res;

    res = dlx::Tokenize("#1000");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "#1000", dlx::Token::Type::ImmediateInteger, 1, 1);

    res = dlx::Tokenize("#-1000");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "#-1000", dlx::Token::Type::ImmediateInteger, 1, 1);

    res = dlx::Tokenize("#0");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "#0", dlx::Token::Type::ImmediateInteger, 1, 1);

    res = dlx::Tokenize("#1000 #12");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "#1000", dlx::Token::Type::ImmediateInteger, 1, 1);
    TokenMatches(res.consume(), "#12", dlx::Token::Type::ImmediateInteger, 1, 7);

    res = dlx::Tokenize("#-1000 #-12");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "#-1000", dlx::Token::Type::ImmediateInteger, 1, 1);
    TokenMatches(res.consume(), "#-12", dlx::Token::Type::ImmediateInteger, 1, 8);
}

TEST_CASE("Tokenize - IntegerLiteral")
{
    dlx::TokenStream res;

    res = dlx::Tokenize("0");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "0", dlx::Token::Type::IntegerLiteral, 1, 1);

    res = dlx::Tokenize("1000");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "1000", dlx::Token::Type::IntegerLiteral, 1, 1);

    res = dlx::Tokenize("-1000");
    REQUIRE(bool(res.size() == 1u));
    TokenMatches(res.consume(), "-1000", dlx::Token::Type::IntegerLiteral, 1, 1);

    res = dlx::Tokenize("1000 2000");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "1000", dlx::Token::Type::IntegerLiteral, 1, 1);
    TokenMatches(res.consume(), "2000", dlx::Token::Type::IntegerLiteral, 1, 6);

    res = dlx::Tokenize("-1000 -2000");
    REQUIRE(bool(res.size() == 2u));
    TokenMatches(res.consume(), "-1000", dlx::Token::Type::IntegerLiteral, 1, 1);
    TokenMatches(res.consume(), "-2000", dlx::Token::Type::IntegerLiteral, 1, 7);
}

TEST_CASE("Tokenize - crashes")
{
    dlx::TokenStream res;

    SECTION("crash-c567e237f4822cff4cab65198f9ea3b393e6f92c tests")
    {
        res = dlx::Tokenize(" \n\n\n:");
        REQUIRE(bool(res.size() == 4u));
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);
        TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 4u, 1u);

        res = dlx::Tokenize(" \n\n\n:x");
        REQUIRE(bool(res.size() == 5u));
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);
        TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 4u, 1u);
        TokenMatches(res.consume(), "x", dlx::Token::Type::LabelIdentifier, 4u, 2u);

        res = dlx::Tokenize(" \n\n\n:x;");
        REQUIRE(bool(res.size() == 6u));
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);
        TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 4u, 1u);
        TokenMatches(res.consume(), "x", dlx::Token::Type::LabelIdentifier, 4u, 2u);
        TokenMatches(res.consume(), ";", dlx::Token::Type::Comment, 4u, 3u);
    }

    SECTION("crash-c567e237f4822cff4cab65198f9ea3b393e6f92c")
    {
        res = dlx::Tokenize(" \n\n\n:x;(");
        REQUIRE(bool(res.size() == 6u));
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 1u, 2u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 2u, 1u);
        TokenMatches(res.consume(), "\n", dlx::Token::Type::NewLine, 3u, 1u);
        TokenMatches(res.consume(), ":", dlx::Token::Type::Colon, 4u, 1u);
        TokenMatches(res.consume(), "x", dlx::Token::Type::LabelIdentifier, 4u, 2u);
        TokenMatches(res.consume(), ";(", dlx::Token::Type::Comment, 4u, 3u);
    }
}
