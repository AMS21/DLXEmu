#include "DLX/ParsedProgram.hpp"

#include <phi/compiler_support/warning.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/move.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

namespace dlx
{
    void ParsedProgram::AddParseError(ParseError&& error) noexcept
    {
        m_ParseErrors.emplace_back(phi::move(error));
    }

    PHI_ATTRIBUTE_CONST phi::boolean ParsedProgram::IsValid() const noexcept
    {
        return m_ParseErrors.empty() && !m_Instructions.empty();
    }

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

    std::string ParsedProgram::GetDump() const noexcept
    {
        std::string text;

        text.append(fmt::format("Valid: {:s}\n\n", IsValid() ? "True" : "False"));

        // Parser errors
        text.append("Parser errors:\n");

        if (m_ParseErrors.empty())
        {
            text.append("None\n");
        }
        else
        {
            for (const ParseError& err : m_ParseErrors)
            {
                text.append(err.ConstructMessage() + '\n');
            }
        }

        // Jump data
        text.append("\nJump data:\n");

        if (m_JumpData.empty())
        {
            text.append("None\n");
        }
        else
        {
            for (auto it = m_JumpData.begin(); it != m_JumpData.end(); ++it)
            {
                text.append(fmt::format("L: {:s}, dst: {:d}\n", it->first, it->second));
            }
        }

        // Instructions
        text.append("\nInstructions:\n");

        if (m_Instructions.empty())
        {
            text.append("None\n");
        }
        else
        {
            for (phi::usize i{0u}; i < m_Instructions.size(); ++i)
            {
                const Instruction& instr = m_Instructions.at(i.unsafe());
                text.append(fmt::format("#{:03d}:  {:s}\n", i.unsafe(), instr.DebugInfo()));
            }
        }

        // Tokens
        text.append("\nTokens:\n");

        if (m_Tokens.empty())
        {
            text.append("None\n");
        }
        else
        {
            for (const Token& token : m_Tokens)
            {
                text.append(token.DebugInfo() + '\n');
            }
        }

        return text;
    }

    PHI_GCC_SUPPRESS_WARNING_POP()
} // namespace dlx
