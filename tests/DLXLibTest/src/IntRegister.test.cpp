#include <phi/test/test_macros.hpp>

#include <DLX/IntRegister.hpp>

TEST_CASE("IntRegister")
{
    dlx::IntRegister reg;

    CHECK(reg.GetSignedValue().unsafe() == 0);
    CHECK(reg.GetUnsignedValue().unsafe() == 0);
    CHECK_FALSE(static_cast<bool>(reg.IsReadOnly()));

    reg.SetSignedValue(32);
    CHECK(reg.GetSignedValue().unsafe() == 32);
    CHECK(reg.GetUnsignedValue().unsafe() == 32);
    CHECK_FALSE(static_cast<bool>(reg.IsReadOnly()));

    reg.SetReadOnly(true);
    CHECK(static_cast<bool>(reg.IsReadOnly()));

    reg.SetReadOnly(false);
    CHECK_FALSE(static_cast<bool>(reg.IsReadOnly()));
}
