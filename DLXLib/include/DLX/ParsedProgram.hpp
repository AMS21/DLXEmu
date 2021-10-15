#pragma once

#include "Instruction.hpp"
#include "Token.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace dlx
{
    struct ParseError
    {
        std::string   message;
        std::uint32_t line_number;
        std::uint32_t column;
    };

    struct ParsedProgram
    {
        std::vector<Instruction>                            m_Instructions;
        std::unordered_map<std::string_view, std::uint32_t> m_JumpData;
        std::vector<ParseError>                             m_ParseErrors;
        std::vector<Token>                                  m_Tokens;

        [[nodiscard]] std::string GetDump() const noexcept;
    };
} // namespace dlx
