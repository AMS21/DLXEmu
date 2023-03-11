#pragma once

#include "DLX/ParseError.hpp"
#include "DLX/TokenStream.hpp"
#include "Instruction.hpp"
#include "Token.hpp"
#include <phi/compiler_support/warning.hpp>
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

        PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

        [[nodiscard]] std::string GetDump() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_POP()
    };
} // namespace dlx
