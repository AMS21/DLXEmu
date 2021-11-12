#pragma once

#include "DLX/TokenStream.hpp"
#include "InstructionLibrary.hpp"
#include "ParsedProgram.hpp"
#include "Token.hpp"
#include <string_view>
#include <vector>

namespace dlx
{
    class Parser
    {
    public:
        static ParsedProgram Parse(const InstructionLibrary& lib, TokenStream& tokens) noexcept;

        static ParsedProgram Parse(const InstructionLibrary& lib, std::string_view source) noexcept;
    };
} // namespace dlx
