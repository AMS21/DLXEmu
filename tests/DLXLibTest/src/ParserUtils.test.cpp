#include <catch2/catch_test_macros.hpp>

#include <DLX/ParserUtils.hpp>

TEST_CASE("IsBeginCommentCharacter")
{
    CHECK(dlx::IsBeginCommentCharacter(';'));
    CHECK(dlx::IsBeginCommentCharacter('/'));

    for (char c{'a'}; c <= 'z'; ++c)
    {
        CHECK_FALSE(dlx::IsBeginCommentCharacter(c));
    }

    for (char c{'A'}; c <= 'Z'; ++c)
    {
        CHECK_FALSE(dlx::IsBeginCommentCharacter(c));
    }

    for (char c{'0'}; c <= '9'; ++c)
    {
        CHECK_FALSE(dlx::IsBeginCommentCharacter(c));
    }

    CHECK_FALSE(dlx::IsBeginCommentCharacter('_'));
    CHECK_FALSE(dlx::IsBeginCommentCharacter('\n'));
    CHECK_FALSE(dlx::IsBeginCommentCharacter(' '));
    CHECK_FALSE(dlx::IsBeginCommentCharacter('\v'));
    CHECK_FALSE(dlx::IsBeginCommentCharacter('\t'));
}

TEST_CASE("IsDigit")
{
    for (char c{'0'}; c <= '9'; ++c)
    {
        CHECK(dlx::IsDigit(c));
    }

    for (char c{'a'}; c <= 'z'; ++c)
    {
        CHECK_FALSE(dlx::IsDigit(c));
    }

    for (char c{'A'}; c <= 'Z'; ++c)
    {
        CHECK_FALSE(dlx::IsDigit(c));
    }

    CHECK_FALSE(dlx::IsDigit('_'));
    CHECK_FALSE(dlx::IsDigit(';'));
    CHECK_FALSE(dlx::IsDigit('/'));
    CHECK_FALSE(dlx::IsDigit('\n'));
    CHECK_FALSE(dlx::IsDigit(' '));
    CHECK_FALSE(dlx::IsDigit('\v'));
    CHECK_FALSE(dlx::IsDigit('\t'));
}

TEST_CASE("IsAlpha")
{
    for (char c{'a'}; c <= 'z'; ++c)
    {
        CHECK(dlx::IsAlpha(c));
    }

    for (char c{'A'}; c <= 'Z'; ++c)
    {
        CHECK(dlx::IsAlpha(c));
    }

    for (char c{'0'}; c <= '9'; ++c)
    {
        CHECK_FALSE(dlx::IsAlpha(c));
    }

    CHECK_FALSE(dlx::IsAlpha('_'));
    CHECK_FALSE(dlx::IsAlpha(';'));
    CHECK_FALSE(dlx::IsAlpha('/'));
    CHECK_FALSE(dlx::IsAlpha('\n'));
    CHECK_FALSE(dlx::IsAlpha(' '));
    CHECK_FALSE(dlx::IsAlpha('\v'));
    CHECK_FALSE(dlx::IsAlpha('\t'));
}

TEST_CASE("IsBlank")
{
    for (char c{'a'}; c <= 'z'; ++c)
    {
        CHECK_FALSE(dlx::IsBlank(c));
    }

    for (char c{'A'}; c <= 'Z'; ++c)
    {
        CHECK_FALSE(dlx::IsBlank(c));
    }

    for (char c{'0'}; c <= '9'; ++c)
    {
        CHECK_FALSE(dlx::IsBlank(c));
    }

    CHECK_FALSE(dlx::IsBlank('_'));
    CHECK_FALSE(dlx::IsBlank(';'));
    CHECK_FALSE(dlx::IsBlank('/'));
    CHECK_FALSE(dlx::IsBlank('\n'));
    CHECK(dlx::IsBlank(' '));
    CHECK(dlx::IsBlank('\t'));
    CHECK_FALSE(dlx::IsBlank('\v'));
    CHECK_FALSE(dlx::IsBlank('\f'));
    CHECK_FALSE(dlx::IsBlank('\r'));
}

TEST_CASE("IsSpace")
{
    CHECK(dlx::IsSpace(' '));
    CHECK(dlx::IsSpace('\v'));
    CHECK(dlx::IsSpace('\t'));
    CHECK(dlx::IsSpace('\v'));
    CHECK(dlx::IsSpace('\f'));
    CHECK(dlx::IsSpace('\r'));

    CHECK_FALSE(dlx::IsSpace('_'));
    CHECK_FALSE(dlx::IsSpace(';'));
    CHECK_FALSE(dlx::IsSpace('/'));

    for (char c{'a'}; c <= 'z'; ++c)
    {
        CHECK_FALSE(dlx::IsSpace(c));
    }

    for (char c{'A'}; c <= 'Z'; ++c)
    {
        CHECK_FALSE(dlx::IsSpace(c));
    }

    for (char c{'0'}; c <= '9'; ++c)
    {
        CHECK_FALSE(dlx::IsSpace(c));
    }
}

TEST_CASE("IsAlphaNumeric")
{
    CHECK_FALSE(dlx::IsAlphaNumeric(' '));
    CHECK_FALSE(dlx::IsAlphaNumeric('\v'));
    CHECK_FALSE(dlx::IsAlphaNumeric('\t'));
    CHECK_FALSE(dlx::IsAlphaNumeric('\v'));
    CHECK_FALSE(dlx::IsAlphaNumeric('\f'));
    CHECK_FALSE(dlx::IsAlphaNumeric('\r'));

    CHECK_FALSE(dlx::IsAlphaNumeric('_'));
    CHECK_FALSE(dlx::IsAlphaNumeric(';'));
    CHECK_FALSE(dlx::IsAlphaNumeric('/'));

    for (char c{'a'}; c <= 'z'; ++c)
    {
        CHECK(dlx::IsAlphaNumeric(c));
    }

    for (char c{'A'}; c <= 'Z'; ++c)
    {
        CHECK(dlx::IsAlphaNumeric(c));
    }

    for (char c{'0'}; c <= '9'; ++c)
    {
        CHECK(dlx::IsAlphaNumeric(c));
    }
}

TEST_CASE("IsBinaryChar")
{
    CHECK(dlx::IsBinaryChar('0'));
    CHECK(dlx::IsBinaryChar('1'));
    CHECK_FALSE(dlx::IsBinaryChar('2'));
}

TEST_CASE("IsOctalChar")
{
    CHECK(dlx::IsOctalChar('0'));
    CHECK(dlx::IsOctalChar('1'));
    CHECK(dlx::IsOctalChar('2'));
    CHECK(dlx::IsOctalChar('3'));
    CHECK(dlx::IsOctalChar('4'));
    CHECK(dlx::IsOctalChar('5'));
    CHECK(dlx::IsOctalChar('6'));
    CHECK(dlx::IsOctalChar('7'));
    CHECK_FALSE(dlx::IsOctalChar('8'));
}

TEST_CASE("IsHexadecimalChar")
{
    CHECK(dlx::IsHexadecimalChar('0'));
    CHECK(dlx::IsHexadecimalChar('1'));
    CHECK(dlx::IsHexadecimalChar('2'));
    CHECK(dlx::IsHexadecimalChar('3'));
    CHECK(dlx::IsHexadecimalChar('4'));
    CHECK(dlx::IsHexadecimalChar('5'));
    CHECK(dlx::IsHexadecimalChar('6'));
    CHECK(dlx::IsHexadecimalChar('7'));
    CHECK(dlx::IsHexadecimalChar('8'));
    CHECK(dlx::IsHexadecimalChar('9'));
    CHECK(dlx::IsHexadecimalChar('a'));
    CHECK(dlx::IsHexadecimalChar('A'));
    CHECK(dlx::IsHexadecimalChar('b'));
    CHECK(dlx::IsHexadecimalChar('B'));
    CHECK(dlx::IsHexadecimalChar('c'));
    CHECK(dlx::IsHexadecimalChar('C'));
    CHECK(dlx::IsHexadecimalChar('d'));
    CHECK(dlx::IsHexadecimalChar('D'));
    CHECK(dlx::IsHexadecimalChar('e'));
    CHECK(dlx::IsHexadecimalChar('E'));
    CHECK(dlx::IsHexadecimalChar('f'));
    CHECK(dlx::IsHexadecimalChar('F'));
    CHECK_FALSE(dlx::IsHexadecimalChar('g'));
    CHECK_FALSE(dlx::IsHexadecimalChar('G'));
}

TEST_CASE("IsReservedIdentifer")
{
    CHECK(dlx::IsReservedIdentifier("ADD"));
    CHECK(dlx::IsReservedIdentifier("R0"));
    CHECK(dlx::IsReservedIdentifier("F0"));
    CHECK(dlx::IsReservedIdentifier("FPSR"));
}

TEST_CASE("IsValidIdentifier")
{
    CHECK(dlx::IsValidIdentifier("a"));
    CHECK(dlx::IsValidIdentifier("a_b_c"));
    CHECK(dlx::IsValidIdentifier("label123"));
    CHECK(dlx::IsValidIdentifier("_abc"));
    CHECK(dlx::IsValidIdentifier("_1"));

    CHECK_FALSE(dlx::IsValidIdentifier("_"));
    CHECK_FALSE(dlx::IsValidIdentifier("1"));
    CHECK_FALSE(dlx::IsValidIdentifier("12"));
    CHECK_FALSE(dlx::IsValidIdentifier("444_123"));
    CHECK_FALSE(dlx::IsValidIdentifier("2asbc"));
    CHECK_FALSE(dlx::IsValidIdentifier(""));
}

TEST_CASE("ParseNumber")
{
    SECTION("Empty")
    {
        CHECK_FALSE(dlx::ParseNumber("").has_value());
    }

    SECTION("Seperators")
    {
        CHECK((dlx::ParseNumber("1'1").value() == 11));

        CHECK_FALSE(dlx::ParseNumber("'1").has_value());
        CHECK_FALSE(dlx::ParseNumber("1'").has_value());
        CHECK_FALSE(dlx::ParseNumber("1''1").has_value());
        CHECK_FALSE(dlx::ParseNumber("0'b1").has_value());
    }

    SECTION("Binary")
    {
        CHECK((dlx::ParseNumber("0b0").value() == 0));
        CHECK((dlx::ParseNumber("0b1").value() == 1));

        CHECK((dlx::ParseNumber("0B0").value() == 0));
        CHECK((dlx::ParseNumber("0B1").value() == 1));

        CHECK((dlx::ParseNumber("0b00").value() == 0));
        CHECK((dlx::ParseNumber("0b10").value() == 2));
        CHECK((dlx::ParseNumber("0b11").value() == 3));

        CHECK((dlx::ParseNumber("0B00").value() == 0));
        CHECK((dlx::ParseNumber("0B10").value() == 2));
        CHECK((dlx::ParseNumber("0B11").value() == 3));

        CHECK((dlx::ParseNumber("0b000").value() == 0));
        CHECK((dlx::ParseNumber("0b001").value() == 1));
        CHECK((dlx::ParseNumber("0b010").value() == 2));
        CHECK((dlx::ParseNumber("0b011").value() == 3));
        CHECK((dlx::ParseNumber("0b100").value() == 4));
        CHECK((dlx::ParseNumber("0b101").value() == 5));
        CHECK((dlx::ParseNumber("0b110").value() == 6));
        CHECK((dlx::ParseNumber("0b111").value() == 7));

        CHECK((dlx::ParseNumber("0B000").value() == 0));
        CHECK((dlx::ParseNumber("0B001").value() == 1));
        CHECK((dlx::ParseNumber("0B010").value() == 2));
        CHECK((dlx::ParseNumber("0B011").value() == 3));
        CHECK((dlx::ParseNumber("0B100").value() == 4));
        CHECK((dlx::ParseNumber("0B101").value() == 5));
        CHECK((dlx::ParseNumber("0B110").value() == 6));
        CHECK((dlx::ParseNumber("0B111").value() == 7));

        CHECK_FALSE(dlx::ParseNumber("-0b").has_value());
        CHECK_FALSE(dlx::ParseNumber("+0b").has_value());
        CHECK_FALSE(dlx::ParseNumber("-b").has_value());
        CHECK_FALSE(dlx::ParseNumber("+b").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b").has_value());
        CHECK_FALSE(dlx::ParseNumber("0B").has_value());
        CHECK_FALSE(dlx::ParseNumber("b").has_value());
        CHECK_FALSE(dlx::ParseNumber("B").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b2").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b3").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b4").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b5").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b6").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b7").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b8").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b9").has_value());
        CHECK_FALSE(dlx::ParseNumber("0ba").has_value());
        CHECK_FALSE(dlx::ParseNumber("0bb").has_value());
        CHECK_FALSE(dlx::ParseNumber("0bc").has_value());
        CHECK_FALSE(dlx::ParseNumber("0bd").has_value());
        CHECK_FALSE(dlx::ParseNumber("0be").has_value());
        CHECK_FALSE(dlx::ParseNumber("0bf").has_value());
        CHECK_FALSE(dlx::ParseNumber("0b11111111'11111111'1").has_value());
        CHECK_FALSE(dlx::ParseNumber("-0b0").has_value());
        CHECK_FALSE(dlx::ParseNumber("-0b1").has_value());
    }

    SECTION("Octal")
    {
        CHECK((dlx::ParseNumber("00").value() == 0));
        CHECK((dlx::ParseNumber("01").value() == 1));
        CHECK((dlx::ParseNumber("02").value() == 2));
        CHECK((dlx::ParseNumber("03").value() == 3));
        CHECK((dlx::ParseNumber("04").value() == 4));
        CHECK((dlx::ParseNumber("05").value() == 5));
        CHECK((dlx::ParseNumber("06").value() == 6));
        CHECK((dlx::ParseNumber("07").value() == 7));

        CHECK((dlx::ParseNumber("010").value() == 8));
        CHECK((dlx::ParseNumber("011").value() == 9));
        CHECK((dlx::ParseNumber("012").value() == 10));
        CHECK((dlx::ParseNumber("013").value() == 11));
        CHECK((dlx::ParseNumber("014").value() == 12));
        CHECK((dlx::ParseNumber("015").value() == 13));
        CHECK((dlx::ParseNumber("016").value() == 14));
        CHECK((dlx::ParseNumber("017").value() == 15));

        CHECK_FALSE(dlx::ParseNumber("08").has_value());
        CHECK_FALSE(dlx::ParseNumber("+01").has_value());
        CHECK_FALSE(dlx::ParseNumber("-01").has_value());
    }

    SECTION("Decimal")
    {
        CHECK((dlx::ParseNumber("0").value() == 0));
        CHECK((dlx::ParseNumber("1").value() == 1));
        CHECK((dlx::ParseNumber("2").value() == 2));
        CHECK((dlx::ParseNumber("3").value() == 3));
        CHECK((dlx::ParseNumber("4").value() == 4));
        CHECK((dlx::ParseNumber("5").value() == 5));
        CHECK((dlx::ParseNumber("6").value() == 6));
        CHECK((dlx::ParseNumber("7").value() == 7));
        CHECK((dlx::ParseNumber("8").value() == 8));
        CHECK((dlx::ParseNumber("9").value() == 9));

        CHECK((dlx::ParseNumber("10").value() == 10));
        CHECK((dlx::ParseNumber("11").value() == 11));
        CHECK((dlx::ParseNumber("12").value() == 12));
        CHECK((dlx::ParseNumber("13").value() == 13));
        CHECK((dlx::ParseNumber("14").value() == 14));
        CHECK((dlx::ParseNumber("15").value() == 15));
        CHECK((dlx::ParseNumber("16").value() == 16));
        CHECK((dlx::ParseNumber("17").value() == 17));
        CHECK((dlx::ParseNumber("18").value() == 18));
        CHECK((dlx::ParseNumber("19").value() == 19));

        CHECK((dlx::ParseNumber("+1").value() == 1));
        CHECK((dlx::ParseNumber("+2").value() == 2));
        CHECK((dlx::ParseNumber("+3").value() == 3));
        CHECK((dlx::ParseNumber("+4").value() == 4));
        CHECK((dlx::ParseNumber("+5").value() == 5));
        CHECK((dlx::ParseNumber("+6").value() == 6));
        CHECK((dlx::ParseNumber("+7").value() == 7));
        CHECK((dlx::ParseNumber("+8").value() == 8));
        CHECK((dlx::ParseNumber("+9").value() == 9));

        CHECK((dlx::ParseNumber("+10").value() == 10));
        CHECK((dlx::ParseNumber("+11").value() == 11));
        CHECK((dlx::ParseNumber("+12").value() == 12));
        CHECK((dlx::ParseNumber("+13").value() == 13));
        CHECK((dlx::ParseNumber("+14").value() == 14));
        CHECK((dlx::ParseNumber("+15").value() == 15));
        CHECK((dlx::ParseNumber("+16").value() == 16));
        CHECK((dlx::ParseNumber("+17").value() == 17));
        CHECK((dlx::ParseNumber("+18").value() == 18));
        CHECK((dlx::ParseNumber("+19").value() == 19));

        CHECK((dlx::ParseNumber("-1").value() == -1));
        CHECK((dlx::ParseNumber("-2").value() == -2));
        CHECK((dlx::ParseNumber("-3").value() == -3));
        CHECK((dlx::ParseNumber("-4").value() == -4));
        CHECK((dlx::ParseNumber("-5").value() == -5));
        CHECK((dlx::ParseNumber("-6").value() == -6));
        CHECK((dlx::ParseNumber("-7").value() == -7));
        CHECK((dlx::ParseNumber("-8").value() == -8));
        CHECK((dlx::ParseNumber("-9").value() == -9));

        CHECK((dlx::ParseNumber("-10").value() == -10));
        CHECK((dlx::ParseNumber("-11").value() == -11));
        CHECK((dlx::ParseNumber("-12").value() == -12));
        CHECK((dlx::ParseNumber("-13").value() == -13));
        CHECK((dlx::ParseNumber("-14").value() == -14));
        CHECK((dlx::ParseNumber("-15").value() == -15));
        CHECK((dlx::ParseNumber("-16").value() == -16));
        CHECK((dlx::ParseNumber("-17").value() == -17));
        CHECK((dlx::ParseNumber("-18").value() == -18));
        CHECK((dlx::ParseNumber("-19").value() == -19));

        CHECK_FALSE(dlx::ParseNumber("+").has_value());
        CHECK_FALSE(dlx::ParseNumber("-").has_value());
        CHECK_FALSE(dlx::ParseNumber("+0").has_value());
        CHECK_FALSE(dlx::ParseNumber("-0").has_value());
        CHECK_FALSE(dlx::ParseNumber("a").has_value());
    }

    SECTION("Hexadecimal")
    {
        CHECK((dlx::ParseNumber("0x0").value() == 0));
        CHECK((dlx::ParseNumber("0x1").value() == 1));
        CHECK((dlx::ParseNumber("0x2").value() == 2));
        CHECK((dlx::ParseNumber("0x3").value() == 3));
        CHECK((dlx::ParseNumber("0x4").value() == 4));
        CHECK((dlx::ParseNumber("0x5").value() == 5));
        CHECK((dlx::ParseNumber("0x6").value() == 6));
        CHECK((dlx::ParseNumber("0x7").value() == 7));
        CHECK((dlx::ParseNumber("0x8").value() == 8));
        CHECK((dlx::ParseNumber("0x9").value() == 9));
        CHECK((dlx::ParseNumber("0xa").value() == 10));
        CHECK((dlx::ParseNumber("0xb").value() == 11));
        CHECK((dlx::ParseNumber("0xc").value() == 12));
        CHECK((dlx::ParseNumber("0xd").value() == 13));
        CHECK((dlx::ParseNumber("0xe").value() == 14));
        CHECK((dlx::ParseNumber("0xf").value() == 15));

        CHECK((dlx::ParseNumber("0x10").value() == 16));
        CHECK((dlx::ParseNumber("0x11").value() == 17));
        CHECK((dlx::ParseNumber("0x12").value() == 18));
        CHECK((dlx::ParseNumber("0x13").value() == 19));
        CHECK((dlx::ParseNumber("0x14").value() == 20));
        CHECK((dlx::ParseNumber("0x15").value() == 21));
        CHECK((dlx::ParseNumber("0x16").value() == 22));
        CHECK((dlx::ParseNumber("0x17").value() == 23));
        CHECK((dlx::ParseNumber("0x18").value() == 24));
        CHECK((dlx::ParseNumber("0x19").value() == 25));
        CHECK((dlx::ParseNumber("0x1a").value() == 26));
        CHECK((dlx::ParseNumber("0x1b").value() == 27));
        CHECK((dlx::ParseNumber("0x1c").value() == 28));
        CHECK((dlx::ParseNumber("0x1d").value() == 29));
        CHECK((dlx::ParseNumber("0x1e").value() == 30));
        CHECK((dlx::ParseNumber("0x1f").value() == 31));

        CHECK((dlx::ParseNumber("0Xa").value() == 10));
        CHECK((dlx::ParseNumber("0xA").value() == 10));
        CHECK((dlx::ParseNumber("0XA").value() == 10));

        CHECK_FALSE(dlx::ParseNumber("0x").has_value());
        CHECK_FALSE(dlx::ParseNumber("0X").has_value());
        CHECK_FALSE(dlx::ParseNumber("+0x").has_value());
        CHECK_FALSE(dlx::ParseNumber("-0x").has_value());
        CHECK_FALSE(dlx::ParseNumber("0xg").has_value());
        CHECK_FALSE(dlx::ParseNumber("0A").has_value());
        CHECK_FALSE(dlx::ParseNumber("0F").has_value());
        CHECK_FALSE(dlx::ParseNumber("A").has_value());
        CHECK_FALSE(dlx::ParseNumber("F").has_value());
    }

    SECTION("Overflow")
    {
        CHECK((dlx::ParseNumber("32767").value() == 32767));
        CHECK_FALSE(dlx::ParseNumber("32768").has_value());
        CHECK_FALSE(dlx::ParseNumber("999999").has_value());
    }

    SECTION("Underflow")
    {
        CHECK((dlx::ParseNumber("-32768").value() == -32768));
        CHECK_FALSE(dlx::ParseNumber("-32769").has_value());
        CHECK_FALSE(dlx::ParseNumber("-999999").has_value());
    }
}
