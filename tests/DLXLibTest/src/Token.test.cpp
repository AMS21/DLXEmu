#include <phi/test/test_macros.hpp>

#include <DLX/RegisterNames.hpp>
#include <DLX/Token.hpp>

TEST_CASE("Token")
{
    SECTION("Comment")
    {
        dlx::Token token{dlx::Token::Type::Comment, "/a", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::Comment);
        CHECK(token.GetTypeName() == "Comment");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 2u));
        CHECK(token.GetText() == "/a");
        CHECK(token.GetTextString() == "/a");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("OpCode")
    {
        dlx::Token token{dlx::Token::Type::OpCode, "ADD", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::OpCode);
        CHECK(token.GetTypeName() == "OpCode");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 3u));
        CHECK(token.GetText() == "ADD");
        CHECK(token.GetTextString() == "ADD");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("OpCode with hint")
    {
        dlx::Token token{dlx::Token::Type::OpCode, "ADD", 1u, 2u,
                         static_cast<phi::uint32_t>(dlx::OpCode::ADD)};

        CHECK(token.GetType() == dlx::Token::Type::OpCode);
        CHECK(token.GetTypeName() == "OpCode");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 3u));
        CHECK(token.GetText() == "ADD");
        CHECK(token.GetTextString() == "ADD");
        REQUIRE(token.HasHint());
        CHECK(token.GetHint() == static_cast<phi::uint32_t>(dlx::OpCode::ADD));
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("LabelIdentifier")
    {
        dlx::Token token{dlx::Token::Type::LabelIdentifier, "label", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::LabelIdentifier);
        CHECK(token.GetTypeName() == "LabelIdentifier");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 5u));
        CHECK(token.GetText() == "label");
        CHECK(token.GetTextString() == "label");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("RegisterInt")
    {
        dlx::Token token{dlx::Token::Type::RegisterInt, "R0", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::RegisterInt);
        CHECK(token.GetTypeName() == "RegisterInt");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 2u));
        CHECK(token.GetText() == "R0");
        CHECK(token.GetTextString() == "R0");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("RegisterInt with hint")
    {
        dlx::Token token{dlx::Token::Type::RegisterInt, "R0", 1u, 2u,
                         static_cast<phi::uint32_t>(dlx::IntRegisterID::R0)};

        CHECK(token.GetType() == dlx::Token::Type::RegisterInt);
        CHECK(token.GetTypeName() == "RegisterInt");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 2u));
        CHECK(token.GetText() == "R0");
        CHECK(token.GetTextString() == "R0");
        REQUIRE(token.HasHint());
        CHECK(token.GetHint() == static_cast<phi::uint32_t>(dlx::IntRegisterID::R0));
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("RegisterFloat")
    {
        dlx::Token token{dlx::Token::Type::RegisterFloat, "F0", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::RegisterFloat);
        CHECK(token.GetTypeName() == "RegisterFloat");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 2u));
        CHECK(token.GetText() == "F0");
        CHECK(token.GetTextString() == "F0");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("RegisterFloat with hint")
    {
        dlx::Token token{dlx::Token::Type::RegisterFloat, "F0", 1u, 2u,
                         static_cast<phi::uint32_t>(dlx::FloatRegisterID::F0)};

        CHECK(token.GetType() == dlx::Token::Type::RegisterFloat);
        CHECK(token.GetTypeName() == "RegisterFloat");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 2u));
        CHECK(token.GetText() == "F0");
        CHECK(token.GetTextString() == "F0");
        REQUIRE(token.HasHint());
        CHECK(token.GetHint() == static_cast<phi::uint32_t>(dlx::FloatRegisterID::F0));
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("RegisterStatus")
    {
        dlx::Token token{dlx::Token::Type::RegisterStatus, "FPSR", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::RegisterStatus);
        CHECK(token.GetTypeName() == "RegisterStatus");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 4u));
        CHECK(token.GetText() == "FPSR");
        CHECK(token.GetTextString() == "FPSR");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("Comma")
    {
        dlx::Token token{dlx::Token::Type::Comma, ",", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::Comma);
        CHECK(token.GetTypeName() == "Comma");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 1u));
        CHECK(token.GetText() == ",");
        CHECK(token.GetTextString() == ",");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("Colon")
    {
        dlx::Token token{dlx::Token::Type::Colon, ":", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::Colon);
        CHECK(token.GetTypeName() == "Colon");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 1u));
        CHECK(token.GetText() == ":");
        CHECK(token.GetTextString() == ":");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("OpenBracket")
    {
        dlx::Token token{dlx::Token::Type::OpenBracket, "(", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::OpenBracket);
        CHECK(token.GetTypeName() == "OpenBracket");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 1u));
        CHECK(token.GetText() == "(");
        CHECK(token.GetTextString() == "(");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("ClosingBracket")
    {
        dlx::Token token{dlx::Token::Type::ClosingBracket, ")", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::ClosingBracket);
        CHECK(token.GetTypeName() == "ClosingBracket");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 1u));
        CHECK(token.GetText() == ")");
        CHECK(token.GetTextString() == ")");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("NewLine")
    {
        dlx::Token token{dlx::Token::Type::NewLine, "\n", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::NewLine);
        CHECK(token.GetTypeName() == "NewLine");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 1u));
        CHECK(token.GetText() == "\n");
        CHECK(token.GetTextString() == "\n");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("ImmediateInteger")
    {
        dlx::Token token{dlx::Token::Type::ImmediateInteger, "#42", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::ImmediateInteger);
        CHECK(token.GetTypeName() == "ImmediateInteger");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 3u));
        CHECK(token.GetText() == "#42");
        CHECK(token.GetTextString() == "#42");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("ImmediateInteger with hint")
    {
        dlx::Token token{dlx::Token::Type::ImmediateInteger, "#42", 1u, 2u, 42};

        CHECK(token.GetType() == dlx::Token::Type::ImmediateInteger);
        CHECK(token.GetTypeName() == "ImmediateInteger");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 3u));
        CHECK(token.GetText() == "#42");
        CHECK(token.GetTextString() == "#42");
        REQUIRE(token.HasHint());
        CHECK(token.GetHint() == 42);
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("ImmediateInteger with hint")
    {
        dlx::Token token{dlx::Token::Type::ImmediateInteger, "#42", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::ImmediateInteger);
        CHECK(token.GetTypeName() == "ImmediateInteger");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 3u));
        CHECK(token.GetText() == "#42");
        CHECK(token.GetTextString() == "#42");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("IntegerLiteral")
    {
        dlx::Token token{dlx::Token::Type::IntegerLiteral, "42", 1u, 2u};

        CHECK(token.GetType() == dlx::Token::Type::IntegerLiteral);
        CHECK(token.GetTypeName() == "IntegerLiteral");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 2u));
        CHECK(token.GetText() == "42");
        CHECK(token.GetTextString() == "42");
        CHECK_FALSE(token.HasHint());
        CHECK_FALSE(token.DebugInfo().empty());
    }

    SECTION("IntegerLiteral with hint")
    {
        dlx::Token token{dlx::Token::Type::IntegerLiteral, "42", 1u, 2u, 42};

        CHECK(token.GetType() == dlx::Token::Type::IntegerLiteral);
        CHECK(token.GetTypeName() == "IntegerLiteral");
        CHECK((token.GetLineNumber() == 1u));
        CHECK((token.GetColumn() == 2u));
        CHECK((token.GetLength() == 2u));
        CHECK(token.GetText() == "42");
        CHECK(token.GetTextString() == "42");
        REQUIRE(token.HasHint());
        CHECK(token.GetHint() == 42);
        CHECK_FALSE(token.DebugInfo().empty());
    }
}
