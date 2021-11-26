#include <catch2/catch_test_macros.hpp>

#include <DLX/ParseError.hpp>
#include <DLX/ParsedProgram.hpp>
#include <DLX/Parser.hpp>

TEST_CASE("ParsedProgram::GetDump()")
{
    // Empty
    dlx::ParsedProgram prog;
    std::string        str;

    str = prog.GetDump();
    CHECK(!str.empty());

    // With error
    prog.AddParseError(dlx::ConstructEmptyLabelParseError(1, 1, "test"));

    str = prog.GetDump();
    CHECK(!str.empty());

    // Normal program
    prog = dlx::Parser::Parse("l: ADD R1 R1 R1\nJ l");

    str = prog.GetDump();
    CHECK(!str.empty());
}
