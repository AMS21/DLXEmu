#include <catch2/catch_test_macros.hpp>

#include <DLX/OpCode.hpp>
#include <magic_enum.hpp>
#include <phi/core/size_t.hpp>
#include <cctype>
#include <cmath>
#include <iostream>

void test_all_variants(std::string_view str, dlx::OpCode opcode) noexcept
{
    for (phi::size_t i{0}; i < static_cast<phi::size_t>(std::pow(2u, str.length())); ++i)
    {
        std::string test_str;
        test_str.reserve(str.length());

        // Construct string
        for (phi::size_t j{0}; j < str.length(); ++j)
        {
            if (i & static_cast<phi::size_t>(std::pow(2u, j)))
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

    CHECK(dlx::StringToOpCode("sw") == dlx::OpCode::SW);
    CHECK(dlx::StringToOpCode("Sw") == dlx::OpCode::SW);
    CHECK(dlx::StringToOpCode("sW") == dlx::OpCode::SW);
    CHECK(dlx::StringToOpCode("SW") == dlx::OpCode::SW);

    // Bad values
    CHECK(dlx::StringToOpCode("") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("NUMBER_OF_ELEMENTS") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("NONE") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("R1") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R2") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R3") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R4") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R5") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R6") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R7") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R8") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R9") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R10") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R11") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R12") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R13") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R14") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R15") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R16") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R17") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R18") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R19") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R20") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R21") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R22") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R23") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R24") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R25") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R26") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R27") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R28") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R29") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R30") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("R31") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("F1") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F2") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F3") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F4") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F5") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F6") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F7") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F8") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F9") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F10") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F11") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F12") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F13") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F14") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F15") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F16") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F17") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F18") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F19") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F20") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F21") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F22") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F23") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F24") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F25") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F26") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F27") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F28") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F29") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F30") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("F31") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode(";") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode(":") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("#") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("/") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("\\") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("!") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("\"") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("'") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode(")") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("(") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("[") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("]") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("{") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("}") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("?") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("%") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("&") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("-") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("_") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode(".") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode(",") == dlx::OpCode::NONE);

    // Maximize coverage
    CHECK(dlx::StringToOpCode("X") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("JX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("LX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("OX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XX") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("ADX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("ANX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("AXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DIX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("EQX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("EXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("GTX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("GXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("JAX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("JXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SNX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SRX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SWX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XXX") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("ADNX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("ADXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("BEQX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("BEXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("BFPX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("BNEX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("BNXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("BXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DIVX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("HALX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("HAXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("HXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("JALX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("JAXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("JXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOVX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MULX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MUXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SEQX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SEXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SGEX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SGTX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLAX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLEX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLLX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLTX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SNEX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SRAX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SRLX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SUBX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SGXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SNXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SRXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SUXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("TRAX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("TRXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("TRXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XORX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XOXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("ZXXX") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("ADDUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("ADDXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("ADXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("AXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DIVUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DIVXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DIXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("DXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MULTX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MULXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MUXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SEQUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SEQXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SEXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLEUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLEXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLTUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SLTXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SNEUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SNEXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SNXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SUBUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SUBXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("SUXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XXXXX") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("CVTD2X") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CVTF2X") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CVTI2X") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CVTDXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CVTFXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CVTIXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CVTXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CVXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("CXXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MULTUX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MULTXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MULXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MUXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MXXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XXXXXX") == dlx::OpCode::NONE);

    CHECK(dlx::StringToOpCode("MOVI2FX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOVFP2X") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOVI2XX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOVFPXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOVIXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOVFXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOVXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MOXXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("MXXXXXX") == dlx::OpCode::NONE);
    CHECK(dlx::StringToOpCode("XXXXXXX") == dlx::OpCode::NONE);
}
