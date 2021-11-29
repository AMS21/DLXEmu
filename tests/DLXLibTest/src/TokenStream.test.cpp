#include <catch2/catch_test_macros.hpp>

#include <DLX/Token.hpp>
#include <DLX/TokenStream.hpp>
#include <type_traits>

TEST_CASE("TokenStream")
{
    SECTION("Traits")
    {
        STATIC_REQUIRE(std::is_nothrow_default_constructible<dlx::TokenStream>::value);
        STATIC_REQUIRE(std::is_nothrow_destructible<dlx::TokenStream>::value);
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
        CHECK(stream.has_x_more(2u));
        const dlx::Token& t1 = stream.consume();
        CHECK(t1.GetType() == dlx::Token::Type::Comma);
        CHECK(t1.GetText() == ",");
        CHECK(bool(t1.GetLineNumber() == 0u));
        CHECK(bool(t1.GetColumn() == 1u));

        REQUIRE(stream.has_more());
        CHECK(stream.has_x_more(1u));
        const dlx::Token& t2 = stream.consume();
        CHECK(t2.GetType() == dlx::Token::Type::OpCode);
        CHECK(t2.GetText() == "ADD");
        CHECK(bool(t2.GetLineNumber() == 3u));
        CHECK(bool(t2.GetColumn() == 5u));

        CHECK(stream.reached_end());
    }

    SECTION("push_back")
    {
        {
            dlx::TokenStream stream;

            stream.push_back({dlx::Token::Type::Comma, ",", 0u, 1u});
            stream.push_back({dlx::Token::Type::OpCode, "ADD", 3u, 5u});

            stream.finalize();

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(2u));
            const dlx::Token& t1 = stream.consume();
            CHECK(t1.GetType() == dlx::Token::Type::Comma);
            CHECK(t1.GetText() == ",");
            CHECK(bool(t1.GetLineNumber() == 0u));
            CHECK(bool(t1.GetColumn() == 1u));

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(1u));
            const dlx::Token& t2 = stream.consume();
            CHECK(t2.GetType() == dlx::Token::Type::OpCode);
            CHECK(t2.GetText() == "ADD");
            CHECK(bool(t2.GetLineNumber() == 3u));
            CHECK(bool(t2.GetColumn() == 5u));

            CHECK(stream.reached_end());
        }

        {
            dlx::TokenStream stream;
            const dlx::Token token1{dlx::Token::Type::Comma, ",", 0u, 1u};
            const dlx::Token token2{dlx::Token::Type::OpCode, "ADD", 3u, 5u};

            stream.push_back(token1);
            stream.push_back(token2);

            stream.finalize();

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(2u));
            const dlx::Token& t1 = stream.consume();
            CHECK(t1.GetType() == dlx::Token::Type::Comma);
            CHECK(t1.GetText() == ",");
            CHECK(bool(t1.GetLineNumber() == 0u));
            CHECK(bool(t1.GetColumn() == 1u));

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(1u));
            const dlx::Token& t2 = stream.consume();
            CHECK(t2.GetType() == dlx::Token::Type::OpCode);
            CHECK(t2.GetText() == "ADD");
            CHECK(bool(t2.GetLineNumber() == 3u));
            CHECK(bool(t2.GetColumn() == 5u));

            CHECK(stream.reached_end());
        }

        {
            dlx::TokenStream stream;
            dlx::Token       token1{dlx::Token::Type::Comma, ",", 0u, 1u};
            dlx::Token       token2{dlx::Token::Type::OpCode, "ADD", 3u, 5u};

            stream.push_back(std::move(token1));
            stream.push_back(std::move(token2));

            stream.finalize();

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(2u));
            const dlx::Token& t1 = stream.consume();
            CHECK(t1.GetType() == dlx::Token::Type::Comma);
            CHECK(t1.GetText() == ",");
            CHECK(bool(t1.GetLineNumber() == 0u));
            CHECK(bool(t1.GetColumn() == 1u));

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(1u));
            const dlx::Token& t2 = stream.consume();
            CHECK(t2.GetType() == dlx::Token::Type::OpCode);
            CHECK(t2.GetText() == "ADD");
            CHECK(bool(t2.GetLineNumber() == 3u));
            CHECK(bool(t2.GetColumn() == 5u));

            CHECK(stream.reached_end());
        }
    }

    SECTION("look_ahead")
    {
        dlx::TokenStream stream;

        stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
        stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

        stream.finalize();

        REQUIRE(stream.has_more());
        CHECK(stream.has_x_more(2u));
        const dlx::Token& t1 = stream.look_ahead();
        CHECK(t1.GetType() == dlx::Token::Type::Comma);
        CHECK(t1.GetText() == ",");
        CHECK(bool(t1.GetLineNumber() == 0u));
        CHECK(bool(t1.GetColumn() == 1u));

        REQUIRE(stream.has_more());
        CHECK(stream.has_x_more(2u));
        const dlx::Token& t2 = stream.consume();
        CHECK(t2.GetType() == dlx::Token::Type::Comma);
        CHECK(t2.GetText() == ",");
        CHECK(bool(t2.GetLineNumber() == 0u));
        CHECK(bool(t2.GetColumn() == 1u));

        REQUIRE(stream.has_more());
        CHECK(stream.has_x_more(1u));
        const dlx::Token& t3 = stream.look_ahead();
        CHECK(t3.GetType() == dlx::Token::Type::OpCode);
        CHECK(t3.GetText() == "ADD");
        CHECK(bool(t3.GetLineNumber() == 3u));
        CHECK(bool(t3.GetColumn() == 5u));

        REQUIRE(stream.has_more());
        CHECK(stream.has_x_more(1u));
        const dlx::Token& t4 = stream.consume();
        CHECK(t4.GetType() == dlx::Token::Type::OpCode);
        CHECK(t4.GetText() == "ADD");
        CHECK(bool(t4.GetLineNumber() == 3u));
        CHECK(bool(t4.GetColumn() == 5u));

        CHECK(stream.reached_end());
    }

    SECTION("reset")
    {
        dlx::TokenStream stream;

        stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
        stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

        stream.finalize();

        {
            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(2u));
            const dlx::Token& t1 = stream.consume();
            CHECK(t1.GetType() == dlx::Token::Type::Comma);
            CHECK(t1.GetText() == ",");
            CHECK(bool(t1.GetLineNumber() == 0u));
            CHECK(bool(t1.GetColumn() == 1u));

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(1u));
            const dlx::Token& t2 = stream.consume();
            CHECK(t2.GetType() == dlx::Token::Type::OpCode);
            CHECK(t2.GetText() == "ADD");
            CHECK(bool(t2.GetLineNumber() == 3u));
            CHECK(bool(t2.GetColumn() == 5u));

            CHECK(stream.reached_end());
        }

        stream.reset();

        {
            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(2u));
            const dlx::Token& t1 = stream.consume();
            CHECK(t1.GetType() == dlx::Token::Type::Comma);
            CHECK(t1.GetText() == ",");
            CHECK(bool(t1.GetLineNumber() == 0u));
            CHECK(bool(t1.GetColumn() == 1u));

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(1u));
            const dlx::Token& t2 = stream.consume();
            CHECK(t2.GetType() == dlx::Token::Type::OpCode);
            CHECK(t2.GetText() == "ADD");
            CHECK(bool(t2.GetLineNumber() == 3u));
            CHECK(bool(t2.GetColumn() == 5u));

            CHECK(stream.reached_end());
        }
    }

    SECTION("skip")
    {
        dlx::TokenStream stream;

        stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
        stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

        stream.finalize();

        {
            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(2u));
            stream.skip();

            REQUIRE(stream.has_more());
            CHECK(stream.has_x_more(1u));
            const dlx::Token& t1 = stream.consume();
            CHECK(t1.GetType() == dlx::Token::Type::OpCode);
            CHECK(t1.GetText() == "ADD");
            CHECK(bool(t1.GetLineNumber() == 3u));
            CHECK(bool(t1.GetColumn() == 5u));

            CHECK(stream.reached_end());
        }

        stream.reset();

        {
            REQUIRE(stream.has_more());

            stream.skip(2u);

            REQUIRE_FALSE(stream.has_more());
            REQUIRE(stream.reached_end());
        }
    }

    SECTION("find_first_token_of_type")
    {
        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_first_token_of_type(dlx::Token::Type::Comma);
            CHECK(t1->GetType() == dlx::Token::Type::Comma);
            CHECK(t1->GetText() == ",");
            CHECK(bool(t1->GetLineNumber() == 0u));
            CHECK(bool(t1->GetColumn() == 1u));

            const dlx::Token* t2 = stream.find_first_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "ADD");
            CHECK(bool(t2->GetLineNumber() == 3u));
            CHECK(bool(t2->GetColumn() == 5u));

            const dlx::Token* t3 =
                    stream.find_first_token_of_type(dlx::Token::Type::ImmediateInteger);
            CHECK(t3 == nullptr);
        }

        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "SUB", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_first_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t1->GetType() == dlx::Token::Type::OpCode);
            CHECK(t1->GetText() == "ADD");
            CHECK(bool(t1->GetLineNumber() == 0u));
            CHECK(bool(t1->GetColumn() == 1u));

            stream.skip();

            const dlx::Token* t2 = stream.find_first_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "ADD");
            CHECK(bool(t2->GetLineNumber() == 0u));
            CHECK(bool(t2->GetColumn() == 1u));

            stream.skip();

            const dlx::Token* t3 = stream.find_first_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t3->GetType() == dlx::Token::Type::OpCode);
            CHECK(t3->GetText() == "ADD");
            CHECK(bool(t3->GetLineNumber() == 0u));
            CHECK(bool(t3->GetColumn() == 1u));
        }
    }

    SECTION("find_last_token_of_type")
    {
        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_last_token_of_type(dlx::Token::Type::Comma);
            CHECK(t1->GetType() == dlx::Token::Type::Comma);
            CHECK(t1->GetText() == ",");
            CHECK(bool(t1->GetLineNumber() == 0u));
            CHECK(bool(t1->GetColumn() == 1u));

            const dlx::Token* t2 = stream.find_last_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "ADD");
            CHECK(bool(t2->GetLineNumber() == 3u));
            CHECK(bool(t2->GetColumn() == 5u));

            const dlx::Token* t3 =
                    stream.find_last_token_of_type(dlx::Token::Type::ImmediateInteger);
            CHECK(t3 == nullptr);
        }

        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "SUB", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_last_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t1->GetType() == dlx::Token::Type::OpCode);
            CHECK(t1->GetText() == "SUB");
            CHECK(bool(t1->GetLineNumber() == 3u));
            CHECK(bool(t1->GetColumn() == 5u));

            stream.skip();

            const dlx::Token* t2 = stream.find_last_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "SUB");
            CHECK(bool(t2->GetLineNumber() == 3u));
            CHECK(bool(t2->GetColumn() == 5u));

            stream.skip();

            const dlx::Token* t3 = stream.find_last_token_of_type(dlx::Token::Type::OpCode);
            CHECK(t3->GetType() == dlx::Token::Type::OpCode);
            CHECK(t3->GetText() == "SUB");
            CHECK(bool(t3->GetLineNumber() == 3u));
            CHECK(bool(t3->GetColumn() == 5u));
        }
    }

    SECTION("find_first_token_if")
    {
        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_first_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::Comma; });
            CHECK(t1->GetType() == dlx::Token::Type::Comma);
            CHECK(t1->GetText() == ",");
            CHECK(bool(t1->GetLineNumber() == 0u));
            CHECK(bool(t1->GetColumn() == 1u));

            const dlx::Token* t2 = stream.find_first_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "ADD");
            CHECK(bool(t2->GetLineNumber() == 3u));
            CHECK(bool(t2->GetColumn() == 5u));

            const dlx::Token* t3 = stream.find_first_token_if([](const dlx::Token& t) {
                return t.GetType() == dlx::Token::Type::ImmediateInteger;
            });
            CHECK(t3 == nullptr);
        }

        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "SUB", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_first_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t1->GetType() == dlx::Token::Type::OpCode);
            CHECK(t1->GetText() == "ADD");
            CHECK(bool(t1->GetLineNumber() == 0u));
            CHECK(bool(t1->GetColumn() == 1u));

            stream.skip();

            const dlx::Token* t2 = stream.find_first_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "ADD");
            CHECK(bool(t2->GetLineNumber() == 0u));
            CHECK(bool(t2->GetColumn() == 1u));

            stream.skip();

            const dlx::Token* t3 = stream.find_first_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t3->GetType() == dlx::Token::Type::OpCode);
            CHECK(t3->GetText() == "ADD");
            CHECK(bool(t3->GetLineNumber() == 0u));
            CHECK(bool(t3->GetColumn() == 1u));
        }
    }

    SECTION("find_last_token_if")
    {
        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::Comma, ",", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_last_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::Comma; });
            CHECK(t1->GetType() == dlx::Token::Type::Comma);
            CHECK(t1->GetText() == ",");
            CHECK(bool(t1->GetLineNumber() == 0u));
            CHECK(bool(t1->GetColumn() == 1u));

            const dlx::Token* t2 = stream.find_last_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "ADD");
            CHECK(bool(t2->GetLineNumber() == 3u));
            CHECK(bool(t2->GetColumn() == 5u));

            const dlx::Token* t3 = stream.find_last_token_if([](const dlx::Token& t) {
                return t.GetType() == dlx::Token::Type::ImmediateInteger;
            });
            CHECK(t3 == nullptr);
        }

        {
            dlx::TokenStream stream;

            stream.emplace_back(dlx::Token::Type::OpCode, "ADD", 0u, 1u);
            stream.emplace_back(dlx::Token::Type::OpCode, "SUB", 3u, 5u);

            stream.finalize();

            const dlx::Token* t1 = stream.find_last_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t1->GetType() == dlx::Token::Type::OpCode);
            CHECK(t1->GetText() == "SUB");
            CHECK(bool(t1->GetLineNumber() == 3u));
            CHECK(bool(t1->GetColumn() == 5u));

            stream.skip();

            const dlx::Token* t2 = stream.find_last_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t2->GetType() == dlx::Token::Type::OpCode);
            CHECK(t2->GetText() == "SUB");
            CHECK(bool(t2->GetLineNumber() == 3u));
            CHECK(bool(t2->GetColumn() == 5u));

            stream.skip();

            const dlx::Token* t3 = stream.find_last_token_if(
                    [](const dlx::Token& t) { return t.GetType() == dlx::Token::Type::OpCode; });
            CHECK(t3->GetType() == dlx::Token::Type::OpCode);
            CHECK(t3->GetText() == "SUB");
            CHECK(bool(t3->GetLineNumber() == 3u));
            CHECK(bool(t3->GetColumn() == 5u));
        }
    }
}
