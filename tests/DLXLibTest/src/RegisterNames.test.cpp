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