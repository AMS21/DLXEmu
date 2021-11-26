#pragma once

#include "DLX/InstructionInfo.hpp"
#include "DLX/OpCode.hpp"

namespace dlx
{
    using InstructionTableT =
            std::array<InstructionInfo, static_cast<std::size_t>(OpCode::NUMBER_OF_ELEMENTS)>;

    [[nodiscard]] InstructionTableT GenerateInstructionTable() noexcept;

    [[nodiscard]] const InstructionInfo& LookUpIntructionInfo(OpCode instruction) noexcept;
} // namespace dlx
