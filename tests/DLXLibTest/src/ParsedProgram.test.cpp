#include <catch2/catch_test_macros.hpp>

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
    dlx::ParseError err;
    err.column      = 1;
    err.line_number = 1;
    err.message     = "Test";
    prog.m_ParseErrors.emplace_back(err);

    str = prog.GetDump();
    CHECK(!str.empty());

    // Normal program
    prog = dlx::Parser::Parse("l: ADD R1 R1 R1\nJ l");

    str = prog.GetDump();
    CHECK(!str.empty());
}
