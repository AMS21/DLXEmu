#include <catch2/catch_test_macros.hpp>

#include <DLX/FloatRegister.hpp>
#include <phi/compiler_support/warning.hpp>

PHI_GCC_SUPPRESS_WARNING("-Wfloat-equal")

TEST_CASE("FloatRegister")
{
    dlx::FloatRegister reg;

    CHECK(reg.GetValue().unsafe() == 0.0f);

    reg.SetValue(21.5f);
    CHECK(reg.GetValue().unsafe() == 21.5f);
}
