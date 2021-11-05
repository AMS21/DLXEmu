#include <catch2/catch_test_macros.hpp>

#include <DLX/OpCode.hpp>
#include <magic_enum.hpp>
#include <cctype>
#include <cmath>
#include <iostream>

void test_all_variants(std::string_view str, dlx::OpCode opcode) noexcept
{
    for (int i{0}; i < std::pow(2, str.length()); ++i)
    {
        std::string test_str;
        test_str.reserve(str.length());

        // Construct string
        for (int j{0}; j < str.length(); ++j)
        {
            if (i & static_cast<int>(std::pow(2, j)))
            {
                test_str += static_cast<char>(std::toupper(str[j]));
            }
            else
            {
                test_str += static_cast<char>(std::tolower(str[j]));
            }
        }

        CHECK(dlx::StringToOpCode(str) == opcode);
    }
}

TEST_CASE("StringToOpCode")
{
    auto codes = magic_enum::enum_entries<dlx::OpCode>();

    for (auto& x : codes)
    {
        // Skip special member
        if (x.first == dlx::OpCode::NUMBER_OF_ELEMENTS)
        {
            continue;
        }

        test_all_variants(x.second, x.first);
    }

    // Bad values
    CHECK(dlx::StringToOpCode("") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("NUMBER_OF_ELEMENTS") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R1") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F1") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode(";") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("/") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("\\") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("!") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("AD") == dlx::OpCode::NONE);
}
