#pragma once

#include "DLX/ParseError.hpp"
#include "DLX/TokenStream.hpp"
#include "Instruction.hpp"
#include "Token.hpp"
#include <phi/core/boolean.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace dlx
{
    struct ParsedProgram
    {
        std::vector<Instruction>                            m_Instructions;
        std::unordered_map<std::string_view, std::uint32_t> m_JumpData;
        std::vector<ParseError>                             m_ParseErrors;
        TokenStream                                         m_Tokens;

        void AddParseError(ParseError&& error) noexcept;

        [[nodiscard]] phi::boolean IsValid() const noexcept;

        [[nodiscard]] std::string GetDump() const noexcept;
    };
} // namespace dlx
