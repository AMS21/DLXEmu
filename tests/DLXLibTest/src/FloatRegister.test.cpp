#include <catch2/catch_test_macros.hpp>

#include <DLX/FloatRegister.hpp>

TEST_CASE("FloatRegister")
{
    dlx::FloatRegister r;

    CHECK(r.GetValue().get() == 0.0f);

    r.SetValue(21.5f);
    CHECK(r.GetValue().get() == 21.5f);
}
