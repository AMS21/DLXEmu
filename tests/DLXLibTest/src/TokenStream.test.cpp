#include "DLX/Token.hpp"
#include <catch2/catch_test_macros.hpp>

#include <DLX/TokenStream.hpp>
#include <type_traits>

TEST_CASE("TokenStream")
{
    SECTION("Traits")
    {
        STATIC_REQUIRE(std::is_nothrow_default_constructible<dlx::TokenStream>::value);
        STATIC_REQUIRE(std::is_copy_constructible<dlx::TokenStream>::value);
        STATIC_REQUIRE(std::is_move_constructible<dlx::TokenStream>::value);
        STATIC_REQUIRE(std::is_copy_assignable<dlx::TokenStream>::value);
        STATIC_REQUIRE(std::is_move_assignable<dlx::TokenStream>::value);
    }

    SECTION("emplace_back")
    {
        dlx::TokenStream stream;

        stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
        stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

        stream.finalize();

        REQUIRE(stream.has_more());
        dlx::Token& token = stream.consume();
        CHECK(token.GetType() == dlx::Token::Type::Comma);
        CHECK(token.GetText() == ",");
        CHECK(bool(token.GetLineNumber() == 0u));
        CHECK(bool(token.GetColumn() == 1u));

        REQUIRE(stream.has_more());
        token = stream.consume();
        CHECK(token.GetType() == dlx::Token::Type::OpCode);
        CHECK(token.GetText() == "ADD");
        CHECK(bool(token.GetLineNumber() == 3u));
        CHECK(bool(token.GetColumn() == 5u));

        CHECK(stream.reached_end());
    }

    SECTION("push_back")
    {
        dlx::TokenStream stream;

        stream.push_back({dlx::Token::Type::Comma, ",", 0u, 1u});
        stream.push_back({dlx::Token::Type::OpCode, "ADD", 3u, 5u});

        stream.finalize();

        REQUIRE(stream.has_more());
        dlx::Token& token = stream.consume();
        CHECK(token.GetType() == dlx::Token::Type::Comma);
        CHECK(token.GetText() == ",");
        CHECK(bool(token.GetLineNumber() == 0u));
        CHECK(bool(token.GetColumn() == 1u));

        REQUIRE(stream.has_more());
        token = stream.consume();
        CHECK(token.GetType() == dlx::Token::Type::OpCode);
        CHECK(token.GetText() == "ADD");
        CHECK(bool(token.GetLineNumber() == 3u));
        CHECK(bool(token.GetColumn() == 5u));

        CHECK(stream.reached_end());
    }

    SECTION("look_ahead")
    {
        dlx::TokenStream stream;

        stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
        stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

        stream.finalize();

        REQUIRE(stream.has_more());
        dlx::Token& token = stream.look_ahead();
        CHECK(token.GetType() == dlx::Token::Type::Comma);
        CHECK(token.GetText() == ",");
        CHECK(bool(token.GetLineNumber() == 0u));
        CHECK(bool(token.GetColumn() == 1u));

        REQUIRE(stream.has_more());
        token = stream.consume();
        CHECK(token.GetType() == dlx::Token::Type::Comma);
        CHECK(token.GetText() == ",");
        CHECK(bool(token.GetLineNumber() == 0u));
        CHECK(bool(token.GetColumn() == 1u));

        REQUIRE(stream.has_more());
        token = stream.look_ahead();
        CHECK(token.GetType() == dlx::Token::Type::OpCode);
        CHECK(token.GetText() == "ADD");
        CHECK(bool(token.GetLineNumber() == 3u));
        CHECK(bool(token.GetColumn() == 5u));

        REQUIRE(stream.has_more());
        token = stream.consume();
        CHECK(token.GetType() == dlx::Token::Type::OpCode);
        CHECK(token.GetText() == "ADD");
        CHECK(bool(token.GetLineNumber() == 3u));
        CHECK(bool(token.GetColumn() == 5u));

        CHECK(stream.reached_end());
    }
}
