#pragma once

#include "Instruction.hpp"
#include "InstructionLibrary.hpp"
#include "Token.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace dlx
{
    struct ParseError
    {
        std::string message;
    };

    struct ParsedProgram
    {
        std::vector<Instruction>                            m_Instructions;
        std::unordered_map<std::string_view, std::uint32_t> m_JumpData;
        std::vector<ParseError>                             m_ParseErrors;
        std::vector<Token>                                  m_Tokens;
    };

    class Parser
    {
    public:
        static std::vector<Token> Tokenize(std::string_view source) noexcept;

        static ParsedProgram Parse(const InstructionLibrary& lib,
                                   std::vector<Token>&       tokens) noexcept;

        static ParsedProgram Parse(const InstructionLibrary& lib, std::string_view source) noexcept;
    };
} // namespace dlx
