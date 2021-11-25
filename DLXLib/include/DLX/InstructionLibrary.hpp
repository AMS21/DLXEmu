#pragma once

#include "DLX/InstructionInfo.hpp"
#include "DLX/OpCode.hpp"

namespace dlx
{
    [[nodiscard]] const InstructionInfo& LookUpIntructionInfo(OpCode instruction) noexcept;
} // namespace dlx
