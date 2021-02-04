#include "DLX/OpCode.hpp"

#include <algorithm>
#include <magic_enum.hpp>

namespace dlx
{
    OpCode StringToOpCode(std::string token)
    {
        std::transform(token.begin(), token.end(), token.begin(), ::toupper);
        std::optional<OpCode> opcode_casted = magic_enum::enum_cast<OpCode>(token);
        if (opcode_casted.has_value())
        {
            // TODO: Bit of a hacky way around magic_enum begin able to cast every member...
            if (opcode_casted.value() == OpCode::NUMBER_OF_ELEMENTS)
            {
                return OpCode::NONE;
            }
            return opcode_casted.value();
        }

        // None found
        return OpCode::NONE;
    }
} // namespace dlx
