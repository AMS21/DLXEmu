#include <catch2/catch_test_macros.hpp>

#include <DLX/IntRegister.hpp>

TEST_CASE("IntRegister")
{
    dlx::IntRegister reg;

    CHECK(reg.GetSignedValue().get() == 0);
    CHECK(reg.GetUnsignedValue().get() == 0);
    CHECK_FALSE(reg.IsReadOnly());

    reg.SetSignedValue(32);
    CHECK(reg.GetSignedValue().get() == 32);
    CHECK(reg.GetUnsignedValue().get() == 32);
    CHECK_FALSE(reg.IsReadOnly());

    reg.SetReadOnly(true);
    CHECK(reg.IsReadOnly());

    reg.SetReadOnly(false);
    CHECK_FALSE(reg.IsReadOnly());
}
