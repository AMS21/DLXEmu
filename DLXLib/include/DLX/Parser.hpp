#pragma once

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
        static std::vector<Token> Tokenize(std::string_view source) noexcept;

        static ParsedProgram Parse(const InstructionLibrary& lib,
                                   std::vector<Token>&       tokens) noexcept;

        static ParsedProgram Parse(const InstructionLibrary& lib, std::string_view source) noexcept;
    };
} // namespace dlx
