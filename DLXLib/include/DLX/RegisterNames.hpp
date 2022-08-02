#pragma once

#include "DLX/EnumName.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <string>

namespace dlx
{

#define DLX_ENUM_INT_REGISTER_ID                                                                   \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R0)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R1)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R2)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R3)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R4)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R5)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R6)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R7)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R8)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R9)                                                              \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R10)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R11)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R12)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R13)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R14)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R15)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R16)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R17)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R18)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R19)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R20)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R21)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R22)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R23)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R24)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R25)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R26)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R27)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R28)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R29)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R30)                                                             \
    DLX_ENUM_INT_REGISTER_ID_IMPL(R31)

#define DLX_ENUM_FLOAT_REGISTER_ID                                                                 \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F0)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F1)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F2)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F3)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F4)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F5)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F6)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F7)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F8)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F9)                                                            \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F10)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F11)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F12)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F13)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F14)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F15)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F16)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F17)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F18)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F19)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F20)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F21)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F22)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F23)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F24)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F25)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F26)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F27)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F28)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F29)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F30)                                                           \
    DLX_ENUM_FLOAT_REGISTER_ID_IMPL(F31)

    enum class IntRegisterID : unsigned
    {
#define DLX_ENUM_INT_REGISTER_ID_IMPL(name) name,

        DLX_ENUM_INT_REGISTER_ID

#undef DLX_ENUM_INT_REGISTER_ID_IMPL
                None,
    };

    template <>
    [[nodiscard]] constexpr std::string_view enum_name<IntRegisterID>(IntRegisterID value) noexcept
    {
        switch (value)
        {
#define DLX_ENUM_INT_REGISTER_ID_IMPL(name)                                                        \
    case IntRegisterID::name:                                                                      \
        return #name;

            DLX_ENUM_INT_REGISTER_ID

#undef DLX_ENUM_INT_REGISTER_ID_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    IntRegisterID StringToIntRegister(std::string_view token) noexcept;

    enum class FloatRegisterID : unsigned
    {
#define DLX_ENUM_FLOAT_REGISTER_ID_IMPL(name) name,
        DLX_ENUM_FLOAT_REGISTER_ID
#undef DLX_ENUM_FLOAT_REGISTER_ID_IMPL

                None,
    };

    template <>
    [[nodiscard]] constexpr std::string_view enum_name<FloatRegisterID>(
            FloatRegisterID value) noexcept
    {
        switch (value)
        {
#define DLX_ENUM_FLOAT_REGISTER_ID_IMPL(name)                                                      \
    case FloatRegisterID::name:                                                                    \
        return #name;

            DLX_ENUM_FLOAT_REGISTER_ID

#undef DLX_ENUM_FLOAT_REGISTER_ID_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    FloatRegisterID StringToFloatRegister(std::string_view token) noexcept;

    bool IsFPSR(std::string_view token) noexcept;
} // namespace dlx
