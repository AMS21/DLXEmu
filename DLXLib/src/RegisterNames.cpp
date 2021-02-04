#include "DLX/RegisterNames.hpp"

#include "DLX/Containers/LookUpMap.hpp"
#include <algorithm>
#include <cctype>

namespace dlx
{
    static constexpr std::array<std::pair<std::string_view, IntRegisterID>, 32> RegisterValues{
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

    IntRegisterID StringToIntRegister(std::string token)
    {
        std::transform(token.begin(), token.end(), token.begin(), ::toupper);

        static constexpr auto map =
                LookUpMap<std::string_view, IntRegisterID, RegisterValues.size()>(RegisterValues, IntRegisterID::None);

        return map.at(token.c_str());
    }
} // namespace dlx
