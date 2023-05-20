#pragma once

#include "DLX/InstructionInfo.hpp"
#include "DLX/OpCode.hpp"
#include <phi/container/array.hpp>

namespace dlx
{
    using InstructionTableT = phi::array<InstructionInfo, NumberOfOpCodes.unsafe()>;

    [[nodiscard]] InstructionTableT GenerateInstructionTable() noexcept;

    [[nodiscard]] const InstructionInfo& LookUpInstructionInfo(OpCode instruction) noexcept;
} // namespace dlx
