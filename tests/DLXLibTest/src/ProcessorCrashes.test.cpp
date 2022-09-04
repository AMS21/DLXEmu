#include <phi/test/test_macros.hpp>

#include <DLX/Logger.hpp>
#include <DLX/ParsedProgram.hpp>
#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>

TEST_CASE("crash-bf9a230d40aedbcc07b5f06dfac1c44f8884c23f")
{
    dlx::ParsedProgram prog = dlx::Parser::Parse("J Z\nJ Z");
    REQUIRE(prog.IsValid());

    dlx::Processor proc;
    proc.LoadProgram(prog);

    proc.ExecuteCurrentProgram();
}
