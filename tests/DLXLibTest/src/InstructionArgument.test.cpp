#include <phi/test/test_macros.hpp>

#include <DLX/InstructionArgument.hpp>
#include <DLX/InstructionInfo.hpp>
#include <DLX/RegisterNames.hpp>

void eq(const dlx::InstructionArgument& lhs, const dlx::InstructionArgument& rhs)
{
    CHECK(lhs == rhs);
    CHECK(rhs == lhs);

    CHECK_FALSE(lhs != rhs);
    CHECK_FALSE(rhs != lhs);
}

void ne(const dlx::InstructionArgument& lhs, const dlx::InstructionArgument& rhs)
{
    CHECK_FALSE(lhs == rhs);
    CHECK_FALSE(rhs == lhs);

    CHECK(lhs != rhs);
    CHECK(rhs != lhs);
}

TEST_CASE("InstructionArgument")
{
    SECTION("Default")
    {
        dlx::InstructionArgument arg;

        CHECK(arg.GetType() == dlx::ArgumentType::None);
        CHECK_FALSE(arg.DebugInfo().empty());
    }

    SECTION("RegisterInt")
    {
        dlx::InstructionArgument arg =
                dlx::ConstructInstructionArgumentRegisterInt(dlx::IntRegisterID::R1);

        CHECK(arg.GetType() == dlx::ArgumentType::IntRegister);
        CHECK_FALSE(arg.DebugInfo().empty());

        const dlx::InstructionArgument::RegisterInt& detail = arg.AsRegisterInt();
        CHECK(detail.register_id == dlx::IntRegisterID::R1);
    }

    SECTION("RegisterFloat")
    {
        dlx::InstructionArgument arg =
                dlx::ConstructInstructionArgumentRegisterFloat(dlx::FloatRegisterID::F1);

        CHECK(arg.GetType() == dlx::ArgumentType::FloatRegister);
        CHECK_FALSE(arg.DebugInfo().empty());

        const dlx::InstructionArgument::RegisterFloat& detail = arg.AsRegisterFloat();
        CHECK(detail.register_id == dlx::FloatRegisterID::F1);
    }

    SECTION("ImmediateValue")
    {
        dlx::InstructionArgument arg = dlx::ConstructInstructionArgumentImmediateValue(42);

        CHECK(arg.GetType() == dlx::ArgumentType::ImmediateInteger);
        CHECK_FALSE(arg.DebugInfo().empty());

        const dlx::InstructionArgument::ImmediateValue& detail = arg.AsImmediateValue();
        CHECK(detail.signed_value == 42);
        CHECK(detail.unsigned_value == 42);
    }

    SECTION("AddressDisplacement")
    {
        dlx::InstructionArgument arg =
                dlx::ConstructInstructionArgumentAddressDisplacement(dlx::IntRegisterID::R0, 42);

        CHECK(arg.GetType() == dlx::ArgumentType::AddressDisplacement);
        CHECK_FALSE(arg.DebugInfo().empty());

        const dlx::InstructionArgument::AddressDisplacement& detail = arg.AsAddressDisplacement();
        CHECK(detail.register_id == dlx::IntRegisterID::R0);
        CHECK(detail.displacement == 42);
    }

    SECTION("Label")
    {
        dlx::InstructionArgument arg = dlx::ConstructInstructionArgumentLabel("l");

        CHECK(arg.GetType() == dlx::ArgumentType::Label);
        CHECK_FALSE(arg.DebugInfo().empty());

        const dlx::InstructionArgument::Label& detail = arg.AsLabel();
        CHECK(detail.label_name == "l");
    }

    SECTION("Comparison")
    {
        dlx::InstructionArgument none;
        dlx::InstructionArgument r0 =
                dlx::ConstructInstructionArgumentRegisterInt(dlx::IntRegisterID::R0);
        dlx::InstructionArgument r1 =
                dlx::ConstructInstructionArgumentRegisterInt(dlx::IntRegisterID::R1);
        dlx::InstructionArgument f0 =
                dlx::ConstructInstructionArgumentRegisterFloat(dlx::FloatRegisterID::F0);
        dlx::InstructionArgument f1 =
                dlx::ConstructInstructionArgumentRegisterFloat(dlx::FloatRegisterID::F1);
        dlx::InstructionArgument i0 = dlx::ConstructInstructionArgumentImmediateValue(0);
        dlx::InstructionArgument i1 = dlx::ConstructInstructionArgumentImmediateValue(1);
        dlx::InstructionArgument r0_0 =
                dlx::ConstructInstructionArgumentAddressDisplacement(dlx::IntRegisterID::R0, 0);
        dlx::InstructionArgument r0_1 =
                dlx::ConstructInstructionArgumentAddressDisplacement(dlx::IntRegisterID::R0, 1);
        dlx::InstructionArgument r1_0 =
                dlx::ConstructInstructionArgumentAddressDisplacement(dlx::IntRegisterID::R1, 0);
        dlx::InstructionArgument r1_1 =
                dlx::ConstructInstructionArgumentAddressDisplacement(dlx::IntRegisterID::R1, 1);
        dlx::InstructionArgument l = dlx::ConstructInstructionArgumentLabel("l");
        dlx::InstructionArgument a = dlx::ConstructInstructionArgumentLabel("a");

        // none
        eq(none, none);
        ne(none, r0);
        ne(none, r1);
        ne(none, f0);
        ne(none, f1);
        ne(none, i0);
        ne(none, i1);
        ne(none, r0_0);
        ne(none, r0_1);
        ne(none, r1_0);
        ne(none, r1_1);
        ne(none, l);
        ne(none, a);

        // r0
        ne(r0, none);
        eq(r0, r0);
        ne(r0, r1);
        ne(r0, f0);
        ne(r0, f1);
        ne(r0, i0);
        ne(r0, i1);
        ne(r0, r0_0);
        ne(r0, r0_1);
        ne(r0, r1_0);
        ne(r0, r1_1);
        ne(r0, l);
        ne(r0, a);

        // r1
        ne(r1, none);
        ne(r1, r0);
        eq(r1, r1);
        ne(r1, f0);
        ne(r1, f1);
        ne(r1, i0);
        ne(r1, i1);
        ne(r1, r0_0);
        ne(r1, r0_1);
        ne(r1, r1_0);
        ne(r1, r1_1);
        ne(r1, l);
        ne(r1, a);

        // f0
        ne(f0, none);
        ne(f0, r0);
        ne(f0, r1);
        eq(f0, f0);
        ne(f0, f1);
        ne(f0, i0);
        ne(f0, i1);
        ne(f0, r0_0);
        ne(f0, r0_1);
        ne(f0, r1_0);
        ne(f0, r1_1);
        ne(f0, l);
        ne(f0, a);

        // f1
        ne(f1, none);
        ne(f1, r0);
        ne(f1, r1);
        ne(f1, f0);
        eq(f1, f1);
        ne(f1, i0);
        ne(f1, i1);
        ne(f1, r0_0);
        ne(f1, r0_1);
        ne(f1, r1_0);
        ne(f1, r1_1);
        ne(f1, l);
        ne(f1, a);

        // i0
        ne(i0, none);
        ne(i0, r0);
        ne(i0, r1);
        ne(i0, f0);
        ne(i0, f1);
        eq(i0, i0);
        ne(i0, i1);
        ne(i0, r0_0);
        ne(i0, r0_1);
        ne(i0, r1_0);
        ne(i0, r1_1);
        ne(i0, l);
        ne(i0, a);

        // i1
        ne(i1, none);
        ne(i1, r0);
        ne(i1, r1);
        ne(i1, f0);
        ne(i1, f1);
        ne(i1, i0);
        eq(i1, i1);
        ne(i1, r0_0);
        ne(i1, r0_1);
        ne(i1, r1_0);
        ne(i1, r1_1);
        ne(i1, l);
        ne(i1, a);

        // r0_0
        ne(r0_0, none);
        ne(r0_0, r0);
        ne(r0_0, r1);
        ne(r0_0, f0);
        ne(r0_0, f1);
        ne(r0_0, i0);
        ne(r0_0, i1);
        eq(r0_0, r0_0);
        ne(r0_0, r0_1);
        ne(r0_0, r1_0);
        ne(r0_0, r1_1);
        ne(r0_0, l);
        ne(r0_0, a);

        // r0_1
        ne(r0_1, none);
        ne(r0_1, r0);
        ne(r0_1, r1);
        ne(r0_1, f0);
        ne(r0_1, f1);
        ne(r0_1, i0);
        ne(r0_1, i1);
        ne(r0_1, r0_0);
        eq(r0_1, r0_1);
        ne(r0_1, r1_0);
        ne(r0_1, r1_1);
        ne(r0_1, l);
        ne(r0_1, a);

        // r1_0
        ne(r1_0, none);
        ne(r1_0, r0);
        ne(r1_0, r1);
        ne(r1_0, f0);
        ne(r1_0, f1);
        ne(r1_0, i0);
        ne(r1_0, i1);
        ne(r1_0, r0_0);
        ne(r1_0, r0_1);
        eq(r1_0, r1_0);
        ne(r1_0, r1_1);
        ne(r1_0, l);
        ne(r1_0, a);

        // r1_1
        ne(r1_1, none);
        ne(r1_1, r0);
        ne(r1_1, r1);
        ne(r1_1, f0);
        ne(r1_1, f1);
        ne(r1_1, i0);
        ne(r1_1, i1);
        ne(r1_1, r0_0);
        ne(r1_1, r0_1);
        ne(r1_1, r1_0);
        eq(r1_1, r1_1);
        ne(r1_1, l);
        ne(r1_1, a);

        // l
        ne(l, none);
        ne(l, r0);
        ne(l, r1);
        ne(l, f0);
        ne(l, f1);
        ne(l, i0);
        ne(l, i1);
        ne(l, r0_0);
        ne(l, r0_1);
        ne(l, r1_0);
        ne(l, r1_1);
        eq(l, l);
        ne(l, a);

        // a
        ne(a, none);
        ne(a, r0);
        ne(a, r1);
        ne(a, f0);
        ne(a, f1);
        ne(a, i0);
        ne(a, i1);
        ne(a, r0_0);
        ne(a, r0_1);
        ne(a, r1_0);
        ne(a, r1_1);
        ne(a, l);
        eq(a, a);
    }
}
