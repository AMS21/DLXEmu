#include <catch2/catch_test_macros.hpp>

#include <DLX/RegisterNames.hpp>

TEST_CASE("StringToIntRegister")
{
    // Valid registers
    CHECK(dlx::StringToIntRegister("R0") == dlx::IntRegisterID::R0);
    CHECK(dlx::StringToIntRegister("R1") == dlx::IntRegisterID::R1);
    CHECK(dlx::StringToIntRegister("R2") == dlx::IntRegisterID::R2);
    CHECK(dlx::StringToIntRegister("R3") == dlx::IntRegisterID::R3);
    CHECK(dlx::StringToIntRegister("R4") == dlx::IntRegisterID::R4);
    CHECK(dlx::StringToIntRegister("R5") == dlx::IntRegisterID::R5);
    CHECK(dlx::StringToIntRegister("R6") == dlx::IntRegisterID::R6);
    CHECK(dlx::StringToIntRegister("R7") == dlx::IntRegisterID::R7);
    CHECK(dlx::StringToIntRegister("R8") == dlx::IntRegisterID::R8);
    CHECK(dlx::StringToIntRegister("R9") == dlx::IntRegisterID::R9);
    CHECK(dlx::StringToIntRegister("R10") == dlx::IntRegisterID::R10);
    CHECK(dlx::StringToIntRegister("R11") == dlx::IntRegisterID::R11);
    CHECK(dlx::StringToIntRegister("R12") == dlx::IntRegisterID::R12);
    CHECK(dlx::StringToIntRegister("R13") == dlx::IntRegisterID::R13);
    CHECK(dlx::StringToIntRegister("R14") == dlx::IntRegisterID::R14);
    CHECK(dlx::StringToIntRegister("R15") == dlx::IntRegisterID::R15);
    CHECK(dlx::StringToIntRegister("R16") == dlx::IntRegisterID::R16);
    CHECK(dlx::StringToIntRegister("R17") == dlx::IntRegisterID::R17);
    CHECK(dlx::StringToIntRegister("R18") == dlx::IntRegisterID::R18);
    CHECK(dlx::StringToIntRegister("R19") == dlx::IntRegisterID::R19);
    CHECK(dlx::StringToIntRegister("R20") == dlx::IntRegisterID::R20);
    CHECK(dlx::StringToIntRegister("R21") == dlx::IntRegisterID::R21);
    CHECK(dlx::StringToIntRegister("R22") == dlx::IntRegisterID::R22);
    CHECK(dlx::StringToIntRegister("R23") == dlx::IntRegisterID::R23);
    CHECK(dlx::StringToIntRegister("R24") == dlx::IntRegisterID::R24);
    CHECK(dlx::StringToIntRegister("R25") == dlx::IntRegisterID::R25);
    CHECK(dlx::StringToIntRegister("R26") == dlx::IntRegisterID::R26);
    CHECK(dlx::StringToIntRegister("R27") == dlx::IntRegisterID::R27);
    CHECK(dlx::StringToIntRegister("R28") == dlx::IntRegisterID::R28);
    CHECK(dlx::StringToIntRegister("R29") == dlx::IntRegisterID::R29);
    CHECK(dlx::StringToIntRegister("R30") == dlx::IntRegisterID::R30);
    CHECK(dlx::StringToIntRegister("R31") == dlx::IntRegisterID::R31);

    // Invalid registers
    CHECK(dlx::StringToIntRegister("R") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R32") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R33") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R34") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R42") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R49") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R57") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R68") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R74") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R86") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R95") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("R100") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("") == dlx::IntRegisterID::None);
    CHECK(dlx::StringToIntRegister("None") == dlx::IntRegisterID::None);
}

TEST_CASE("StringToFloatRegister")
{
    // Valid registers
    CHECK(dlx::StringToFloatRegister("F0") == dlx::FloatRegisterID::F0);
    CHECK(dlx::StringToFloatRegister("F1") == dlx::FloatRegisterID::F1);
    CHECK(dlx::StringToFloatRegister("F2") == dlx::FloatRegisterID::F2);
    CHECK(dlx::StringToFloatRegister("F3") == dlx::FloatRegisterID::F3);
    CHECK(dlx::StringToFloatRegister("F4") == dlx::FloatRegisterID::F4);
    CHECK(dlx::StringToFloatRegister("F5") == dlx::FloatRegisterID::F5);
    CHECK(dlx::StringToFloatRegister("F6") == dlx::FloatRegisterID::F6);
    CHECK(dlx::StringToFloatRegister("F7") == dlx::FloatRegisterID::F7);
    CHECK(dlx::StringToFloatRegister("F8") == dlx::FloatRegisterID::F8);
    CHECK(dlx::StringToFloatRegister("F9") == dlx::FloatRegisterID::F9);
    CHECK(dlx::StringToFloatRegister("F10") == dlx::FloatRegisterID::F10);
    CHECK(dlx::StringToFloatRegister("F11") == dlx::FloatRegisterID::F11);
    CHECK(dlx::StringToFloatRegister("F12") == dlx::FloatRegisterID::F12);
    CHECK(dlx::StringToFloatRegister("F13") == dlx::FloatRegisterID::F13);
    CHECK(dlx::StringToFloatRegister("F14") == dlx::FloatRegisterID::F14);
    CHECK(dlx::StringToFloatRegister("F15") == dlx::FloatRegisterID::F15);
    CHECK(dlx::StringToFloatRegister("F16") == dlx::FloatRegisterID::F16);
    CHECK(dlx::StringToFloatRegister("F17") == dlx::FloatRegisterID::F17);
    CHECK(dlx::StringToFloatRegister("F18") == dlx::FloatRegisterID::F18);
    CHECK(dlx::StringToFloatRegister("F19") == dlx::FloatRegisterID::F19);
    CHECK(dlx::StringToFloatRegister("F20") == dlx::FloatRegisterID::F20);
    CHECK(dlx::StringToFloatRegister("F21") == dlx::FloatRegisterID::F21);
    CHECK(dlx::StringToFloatRegister("F22") == dlx::FloatRegisterID::F22);
    CHECK(dlx::StringToFloatRegister("F23") == dlx::FloatRegisterID::F23);
    CHECK(dlx::StringToFloatRegister("F24") == dlx::FloatRegisterID::F24);
    CHECK(dlx::StringToFloatRegister("F25") == dlx::FloatRegisterID::F25);
    CHECK(dlx::StringToFloatRegister("F26") == dlx::FloatRegisterID::F26);
    CHECK(dlx::StringToFloatRegister("F27") == dlx::FloatRegisterID::F27);
    CHECK(dlx::StringToFloatRegister("F28") == dlx::FloatRegisterID::F28);
    CHECK(dlx::StringToFloatRegister("F29") == dlx::FloatRegisterID::F29);
    CHECK(dlx::StringToFloatRegister("F30") == dlx::FloatRegisterID::F30);
    CHECK(dlx::StringToFloatRegister("F31") == dlx::FloatRegisterID::F31);

    // Invalid registers
    CHECK(dlx::StringToFloatRegister("F") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F32") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F33") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F34") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F42") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F49") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F57") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F68") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F74") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F86") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F95") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("F100") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("") == dlx::FloatRegisterID::None);
    CHECK(dlx::StringToFloatRegister("None") == dlx::FloatRegisterID::None);
}
