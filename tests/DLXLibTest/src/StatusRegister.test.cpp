#include <phi/test/test_macros.hpp>

#include <DLX/StatusRegister.hpp>

TEST_CASE("StatusRegister")
{
    dlx::StatusRegister reg;

    CHECK_FALSE(reg.Get());

    reg.SetStatus(false);

    CHECK_FALSE(reg.Get());

    reg.SetStatus(false);

    CHECK_FALSE(reg.Get());

    reg.SetStatus(true);

    CHECK(reg.Get());

    reg.SetStatus(true);

    CHECK(reg.Get());
}
