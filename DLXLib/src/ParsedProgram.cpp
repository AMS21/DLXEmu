#include "DLX/ParsedProgram.hpp"

namespace dlx
{
    std::string ParsedProgram::GetDump() const noexcept
    {
        std::string text;

        bool valid = !m_Instructions.empty() && m_ParseErrors.empty();

        text.append(fmt::format("Valid: {:s}\n\n", valid ? "True" : "False"));

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
                text.append(err.message + '\n');
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
                const Instruction& instr = m_Instructions.at(i.get());
                text.append(fmt::format("#{:03d}:  {:s}\n", i.get(), instr.DebugInfo()));
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
} // namespace dlx
