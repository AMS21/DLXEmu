#pragma once

#include "OpCode.hpp"
#include "RegisterNames.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/types.hpp>
#include <limits>
#include <string_view>

namespace dlx
{
    /* Character functions */

    constexpr phi::boolean IsBeginCommentCharacter(const char c) noexcept
    {
        return (c == ';') || (c == '/');
    }

    constexpr phi::boolean IsDigit(const char c) noexcept
    {
        return (c >= '0' && c <= '9');
    }

    constexpr phi::boolean IsAlpha(const char c) noexcept
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    constexpr phi::boolean IsBlank(const char c) noexcept
    {
        switch (c)
        {
            case ' ':
            case '\t':
                return true;
            default:
                return false;
        }
    }

    constexpr phi::boolean IsSpace(const char c) noexcept
    {
        switch (c)
        {
            case ' ':
            case '\t':
            case '\n':
            case '\v':
            case '\f':
            case '\r':
                return true;
            default:
                return false;
        }
    }

    constexpr phi::boolean IsAlphaNumeric(const char c) noexcept
    {
        return IsDigit(c) || IsAlpha(c);
    }

    constexpr phi::boolean IsBinaryChar(const char c) noexcept
    {
        return (c == '0') || (c == '1');
    }

    constexpr phi::boolean IsOctalChar(const char c) noexcept
    {
        return (c >= '0') && (c <= '7');
    }

    constexpr phi::boolean IsHexadecimalChar(const char c) noexcept
    {
        return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    constexpr std::uint8_t HexCharValue(const char c) noexcept
    {
        if (IsDigit(c))
        {
            return c - '0';
        }

        if (c >= 'a' && c <= 'f')
        {
            return 10 + (c - 'a');
        }

        if (c >= 'A' && c <= 'F')
        {
            return 10 + (c - 'A');
        }

#if !defined(DLXEMU_COVERAGE_BUILD)
        PHI_DBG_ASSERT_NOT_REACHED();
#endif
    }

    /* String functions */

    [[nodiscard]] inline phi::boolean IsReservedIdentifier(std::string_view token) noexcept
    {
        if (StringToIntRegister(token) != IntRegisterID::None)
        {
            return true;
        }

        if (StringToFloatRegister(token) != FloatRegisterID::None)
        {
            return true;
        }

        if (StringToOpCode(token) != OpCode::NONE)
        {
            return true;
        }

        if (IsFPSR(token))
        {
            return true;
        }

        return false;
    }

    constexpr phi::boolean IsValidIdentifier(std::string_view token) noexcept
    {
        if (token.empty())
        {
            return false;
        }

        const char first_char = token.at(0);

        if (token.length() == 1u)
        {
            return IsAlpha(first_char);
        }

        phi::boolean just_under_scores = (first_char == '_');
        if (!(IsAlpha(first_char) || (first_char == '_')))
        {
            return false;
        }

        for (std::size_t index{1u}; index < token.length(); ++index)
        {
            const char c{token.at(index)};

            if (IsAlpha(c) || IsDigit(c))
            {
                just_under_scores = false;
            }
            else if (c == '_')
            {
                continue;
            }
            else
            {
                return false;
            }
        }

        return !just_under_scores;
    }

    /* Parsing functions */

    constexpr phi::optional<phi::i16> ParseNumber(std::string_view token) noexcept
    {
        if (token.empty())
        {
            return {};
        }

        if (token.length() == 1)
        {
            if (IsDigit(token.at(0)))
            {
                return static_cast<std::int16_t>(token.at(0) - '0');
            }

            return {};
        }

        // Disallow trailing seperators
        if (token.at(token.size() - 1) == '\'')
        {
            return {};
        }

        std::int32_t number{0};
        bool         is_negative{false};
        bool         starts_with_zero{false};
        bool         parsing_binary{false};
        bool         parsing_octal{false};
        bool         parsing_hexadecimal{false};
        bool         parsed_something{false};
        bool         parsed_seperator{false};

        // TODO: Move x'th character checks out of the for loop

        for (std::size_t index{0u}; index < token.length(); ++index)
        {
            const char c{token.at(index)};

            // First character
            if (index == 0u)
            {
                if (c == '+')
                {
                    continue;
                }

                if (c == '-')
                {
                    is_negative = true;
                    continue;
                }

                if (c == '0')
                {
                    starts_with_zero = true;
                    continue;
                }

                if (c == '\'')
                {
                    return {};
                }
            }

            // Second character
            if (index == 1u)
            {
                if (starts_with_zero)
                {
                    // Binary
                    if (c == 'b' || c == 'B')
                    {
                        parsing_binary = true;
                        continue;
                    }

                    // Hexadecimal
                    if (c == 'x' || c == 'X')
                    {
                        parsing_hexadecimal = true;
                        continue;
                    }

                    // Octal
                    parsing_octal = true;
                }
            }

            // Handle seperators
            if (c == '\'')
            {
                if (parsed_seperator)
                {
                    return {};
                }

                parsed_seperator = true;
                continue;
            }

            // Check for over/underflow
            if (is_negative && (-number < std::numeric_limits<std::int16_t>::min()))
            {
                // Would underflow
                return {};
            }
            if (!is_negative && (number > std::numeric_limits<std::int16_t>::max()))
            {
                // Would overflow
                return {};
            }

            parsed_seperator = false;
            parsed_something = true;

            if (parsing_binary)
            {
                if (!IsBinaryChar(c))
                {
                    return {};
                }

                number <<= 1;
                number |= (c - '0');
            }
            else if (parsing_octal)
            {
                if (!IsOctalChar(c))
                {
                    return {};
                }

                number <<= 3;
                number |= (c - '0');
            }
            else if (parsing_hexadecimal)
            {
                if (!IsHexadecimalChar(c))
                {
                    return {};
                }

                number <<= 4;
                number |= HexCharValue(c);
            }
            else
            {
                if (!IsDigit(c))
                {
                    return {};
                }

                // Disallow leading zeros
                if (number == 0 && c == '0')
                {
                    return {};
                }

                number *= 10;
                number += (c - '0');
            }
        }

        if (parsed_something)
        {
            // Check for over/underflow
            if (is_negative && (-number < std::numeric_limits<std::int16_t>::min()))
            {
                // Would underflow
                return {};
            }
            if (!is_negative && (number > std::numeric_limits<std::int16_t>::max()))
            {
                // Would overflow
                return {};
            }

            if (is_negative)
            {
                return static_cast<std::int16_t>(-number);
            }

            return static_cast<std::int16_t>(number);
        }

        return {};
    }

} // namespace dlx
