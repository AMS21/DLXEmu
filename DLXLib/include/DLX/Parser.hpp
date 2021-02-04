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
        std::vector<Instruction>                     m_Instructions;
        // TODO: Do we really need to have a string as the key? Would a string_view be enough
        std::unordered_map<std::string, std::size_t> m_JumpData;
        std::vector<ParseError>                      m_ParseErrors;
    };

    class Parser
    {
    public:
        static std::vector<Token> Tokenize(std::string_view source);

        static ParsedProgram Parse(const InstructionLibrary& lib, std::vector<Token>& tokens);

        static ParsedProgram Parse(const InstructionLibrary& lib, std::string_view source);
    };
} // namespace dlx
