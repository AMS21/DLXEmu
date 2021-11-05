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
    CHECK(dlx::StringToOpCode("OA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("AAA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DDD") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("EEE") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("TTT") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("LBA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("LEA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("LHA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("LTA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("LWW") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("LXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("NEA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("NXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("ORA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SBA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SEA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SGA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SHA") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLB") == dlx::OpCode::NONE);
}
