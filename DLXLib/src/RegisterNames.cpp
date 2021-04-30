#include "DLX/RegisterNames.hpp"

#include "DLX/Containers/LookUpMap.hpp"
#include <algorithm>
#include <cctype>
#include <string_view>

namespace dlx
{
    static constexpr std::array<std::pair<std::string_view, IntRegisterID>, 32> IntRegisterValues{
            {{"R0", IntRegisterID::R0},   {"R1", IntRegisterID::R1},   {"R2", IntRegisterID::R2},
             {"R3", IntRegisterID::R3},   {"R4", IntRegisterID::R4},   {"R5", IntRegisterID::R5},
             {"R6", IntRegisterID::R6},   {"R7", IntRegisterID::R7},   {"R8", IntRegisterID::R8},
             {"R9", IntRegisterID::R9},   {"R10", IntRegisterID::R10}, {"R11", IntRegisterID::R11},
             {"R12", IntRegisterID::R12}, {"R13", IntRegisterID::R13}, {"R14", IntRegisterID::R14},
             {"R15", IntRegisterID::R15}, {"R16", IntRegisterID::R16}, {"R17", IntRegisterID::R17},
             {"R18", IntRegisterID::R18}, {"R19", IntRegisterID::R19}, {"R20", IntRegisterID::R20},
             {"R21", IntRegisterID::R21}, {"R22", IntRegisterID::R22}, {"R23", IntRegisterID::R23},
             {"R24", IntRegisterID::R24}, {"R25", IntRegisterID::R25}, {"R26", IntRegisterID::R26},
             {"R27", IntRegisterID::R27}, {"R28", IntRegisterID::R28}, {"R29", IntRegisterID::R29},
             {"R30", IntRegisterID::R30}, {"R31", IntRegisterID::R31}}};

    static constexpr std::array<std::pair<std::string_view, FloatRegisterID>, 32>
            FloatRegisterValues{{{"F0", FloatRegisterID::F0},   {"F1", FloatRegisterID::F1},
                                 {"F2", FloatRegisterID::F2},   {"F3", FloatRegisterID::F3},
                                 {"F4", FloatRegisterID::F4},   {"F5", FloatRegisterID::F5},
                                 {"F6", FloatRegisterID::F6},   {"F7", FloatRegisterID::F7},
                                 {"F8", FloatRegisterID::F8},   {"F9", FloatRegisterID::F9},
                                 {"F10", FloatRegisterID::F10}, {"F11", FloatRegisterID::F11},
                                 {"F12", FloatRegisterID::F12}, {"F13", FloatRegisterID::F13},
                                 {"F14", FloatRegisterID::F14}, {"F15", FloatRegisterID::F15},
                                 {"F16", FloatRegisterID::F16}, {"F17", FloatRegisterID::F17},
                                 {"F18", FloatRegisterID::F18}, {"F19", FloatRegisterID::F19},
                                 {"F20", FloatRegisterID::F20}, {"F21", FloatRegisterID::F21},
                                 {"F22", FloatRegisterID::F22}, {"F23", FloatRegisterID::F23},
                                 {"F24", FloatRegisterID::F24}, {"F25", FloatRegisterID::F25},
                                 {"F26", FloatRegisterID::F26}, {"F27", FloatRegisterID::F27},
                                 {"F28", FloatRegisterID::F28}, {"F29", FloatRegisterID::F29},
                                 {"F30", FloatRegisterID::F30}, {"F31", FloatRegisterID::F31}}};

    IntRegisterID StringToIntRegister(std::string_view token) noexcept
    {
        static constexpr auto map =
                LookUpMap<std::string_view, IntRegisterID, IntRegisterValues.size()>(
                        IntRegisterValues, IntRegisterID::None);

        return map.at(token);
    }

    FloatRegisterID StringToFloatRegister(std::string_view token) noexcept
    {
        static constexpr auto map =
                LookUpMap<std::string_view, FloatRegisterID, FloatRegisterValues.size()>(
                        FloatRegisterValues, FloatRegisterID::None);

        return map.at(token);
    }
} // namespace dlx
