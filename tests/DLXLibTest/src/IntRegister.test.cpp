#include <phi/test/test_macros.hpp>

#include <DLX/IntRegister.hpp>

TEST_CASE("IntRegister")
{
    dlx::IntRegister reg;

    CHECK(reg.GetSignedValue() == 0);
    CHECK(reg.GetUnsignedValue() == 0u);
    CHECK_FALSE(static_cast<bool>(reg.IsReadOnly()));

    reg.SetSignedValue(32);
    CHECK(reg.GetSignedValue() == 32);
    CHECK(reg.GetUnsignedValue() == 32u);
    CHECK_FALSE(static_cast<bool>(reg.IsReadOnly()));

    reg.SetReadOnly(true);
    CHECK(static_cast<bool>(reg.IsReadOnly()));

    reg.SetReadOnly(false);
    CHECK_FALSE(static_cast<bool>(reg.IsReadOnly()));
}
