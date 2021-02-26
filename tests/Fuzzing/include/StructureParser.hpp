#pragma once

#include <DLX/OpCode.hpp>
#include <DLX/ParserUtils.hpp>
#include <DLX/RegisterNames.hpp>
#include <magic_enum.hpp>
#include <algorithm>
#include <cstdint>
#include <string>

namespace fuzz
{
    namespace detail
    {
        inline void AddSeperatorToken(std::string& text, const std::uint8_t* data, std::size_t size,
                                      std::size_t& index)
        {
            if (index < size)
            {
                if (data[index++] % 2 == 0)
                {
                    text += ' ';
                }
                else
                {
                    text += ',';
                }
            }
        }

        inline char SanitizeForIdentifier(std::uint8_t c)
        {
            if (!dlx::IsAlphaNumeric(c))
            {
                return '_';
            }

            return c;
        }
    } // namespace detail

    inline std::string ParseAsStrucutedDLXCode(const std::uint8_t* data, std::size_t size)
    {
        // Constants
        constexpr std::uint8_t number_of_opcodes =
                static_cast<std::uint8_t>(dlx::OpCode::NUMBER_OF_ELEMENTS);
        constexpr std::uint8_t number_of_int_registers   = 32;
        constexpr std::uint8_t number_of_float_registers = 32;

        std::string ret;

        for (std::size_t index{0}; index < size;)
        {
            std::uint8_t current_value = data[index++];

            switch (current_value)
            {
                // OpCode
                case 0: {
                    // Has one more value
                    if (index < size)
                    {
                        std::uint8_t opcode_value = data[index++] % number_of_opcodes;

                        ret += magic_enum::enum_name(static_cast<dlx::OpCode>(opcode_value));

                        detail::AddSeperatorToken(ret, data, size, index);
                    }
                    break;
                }

                // Int Register
                case 1: {
                    // Has one more value
                    if (index < size)
                    {
                        std::uint8_t opcode_value = data[index++] % number_of_int_registers;

                        ret += magic_enum::enum_name(static_cast<dlx::IntRegisterID>(opcode_value));
                        detail::AddSeperatorToken(ret, data, size, index);
                    }
                    break;
                }

                // Float register
                case 2: {
                    // Has one more value
                    if (index < size)
                    {
                        std::uint8_t opcode_value = data[index++] % number_of_float_registers;

                        ret += magic_enum::enum_name(
                                static_cast<dlx::FloatRegisterID>(opcode_value));
                        detail::AddSeperatorToken(ret, data, size, index);
                    }
                    break;
                }

                // Floating point status register
                case 3: {
                    ret += "FPSR";
                    detail::AddSeperatorToken(ret, data, size, index);

                    break;
                }

                // Immediate integer
                case 4: {
                    ret += "#";
                    [[fallthrough]];
                }

                // Integer literal
                case 5: {
                    std::size_t size_of_int = std::min(2ul, size - index);

                    if (size_of_int > 0)
                    {
                        std::int16_t value = 0;

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
                    std::size_t label_length = std::min(5ul, size - index);

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
                            detail::AddSeperatorToken(ret, data, size, index);
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
                    std::size_t comment_length = std::min(5ul, size - index);

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
                    break;
            }
        }

        return ret;
    }
} // namespace fuzz
