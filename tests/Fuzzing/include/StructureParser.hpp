#pragma once

#include <DLX/OpCode.hpp>
#include <DLX/ParserUtils.hpp>
#include <DLX/RegisterNames.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/text/is_alpha_numeric.hpp>
#include <algorithm>
#include <string>

namespace fuzz
{
    namespace detail
    {
        constexpr char ErrorString[] = "";

        PHI_CLANG_SUPPRESS_WARNING_PUSH()
        PHI_CLANG_SUPPRESS_WARNING("-Wunknown-warning-option")
        PHI_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")

        inline bool AddSeperatorToken(std::string& text, const phi::uint8_t* data, phi::size_t size,
                                      phi::size_t& index)
        {
            if (index < size)
            {
                phi::uint8_t val = data[index++];
                if (val == 0)
                {
                    text += ' ';
                }
                else if (val == 1)
                {
                    text += ',';
                }
                else
                {
                    return false;
                }
            }

            return true;
        }

        PHI_CLANG_SUPPRESS_WARNING_POP()

        inline char SanitizeForIdentifier(phi::uint8_t c)
        {
            if (!phi::is_alpha_numeric(static_cast<char>(c)))
            {
                return '_';
            }

            return c;
        }
    } // namespace detail

    inline std::string ParseAsStrucutedDLXCode(const phi::uint8_t* data, phi::size_t size)
    {
        // Constants
        constexpr phi::uint8_t number_of_opcodes =
                static_cast<phi::uint8_t>(dlx::OpCode::NUMBER_OF_ELEMENTS);
        constexpr phi::uint8_t number_of_int_registers   = 32;
        constexpr phi::uint8_t number_of_float_registers = 32;

        std::string ret;

        for (phi::size_t index{0}; index < size;)
        {
            phi::uint8_t current_value = data[index++];

            switch (current_value)
            {
                // OpCode
                case 0: {
                    // Has one more value
                    if (index < size)
                    {
                        phi::uint8_t opcode_value = data[index++] % number_of_opcodes;

                        ret += dlx::enum_name(static_cast<dlx::OpCode>(opcode_value));

                        if (!detail::AddSeperatorToken(ret, data, size, index))
                        {
                            return detail::ErrorString;
                        }
                    }
                    break;
                }

                // Int Register
                case 1: {
                    // Has one more value
                    if (index < size)
                    {
                        phi::uint8_t opcode_value = data[index++] % number_of_int_registers;

                        ret += dlx::enum_name(static_cast<dlx::IntRegisterID>(opcode_value));
                        if (!detail::AddSeperatorToken(ret, data, size, index))
                        {
                            return detail::ErrorString;
                        }
                    }
                    break;
                }

                // Float register
                case 2: {
                    // Has one more value
                    if (index < size)
                    {
                        phi::uint8_t opcode_value = data[index++] % number_of_float_registers;

                        ret += dlx::enum_name(static_cast<dlx::FloatRegisterID>(opcode_value));
                        if (!detail::AddSeperatorToken(ret, data, size, index))
                        {
                            return detail::ErrorString;
                        }
                    }
                    break;
                }

                // Floating point status register
                case 3: {
                    ret += "FPSR";
                    if (!detail::AddSeperatorToken(ret, data, size, index))
                    {
                        return detail::ErrorString;
                    }

                    break;
                }

                // Immediate integer
                case 4: {
                    ret += "#";
                    [[fallthrough]];
                }

                // Integer literal
                case 5: {
                    phi::size_t size_of_int = std::min(2ul, size - index);

                    if (size_of_int > 0)
                    {
                        phi::int16_t value = 0;

                        for (; size_of_int > 0; --size_of_int)
                        {
                            value <<= 8;
                            value &= data[index++];
                        }

                        ret += std::to_string(value);
                    }
                    break;
                }

                // Label
                case 6: {
                    phi::size_t label_length = std::min(5ul, size - index);

                    if (label_length > 0)
                    {
                        std::string label_name = "lbl_";

                        for (; label_length > 0; --label_length)
                        {
                            char c = detail::SanitizeForIdentifier(data[index++]);

                            label_name += c;
                        }

                        // Add 1 in 10 chance to add a colon
                        if (index < size && data[index++] % 10 == 0)
                        {
                            label_name += ':';
                            ret += label_name;
                        }
                        else
                        {
                            ret += label_name;
                            if (!detail::AddSeperatorToken(ret, data, size, index))
                            {
                                return detail::ErrorString;
                            }
                        }
                    }
                    break;
                }

                // Newline
                case 7: {
                    ret += '\n';
                    break;
                }

                // Comment
                case 8: {
                    phi::size_t comment_length = std::min(5ul, size - index);

                    if (comment_length > 0)
                    {
                        std::string comment = "; ";

                        for (; comment_length > 0; --comment_length)
                        {
                            comment += static_cast<char>(data[index++]);
                        }

                        ret += comment + '\n';
                    }
                    break;
                }

                // Ignore
                default:
                    return detail::ErrorString;
            }
        }

        return ret;
    }
} // namespace fuzz
