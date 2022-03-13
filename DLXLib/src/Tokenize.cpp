#include "DLX/Tokenize.hpp"

#include "DLX/ParserUtils.hpp"
#include "DLX/TokenStream.hpp"
#include <string_view>

namespace dlx
{
    static Token ParseToken(std::string_view token, phi::u64 line_number, phi::u64 column) noexcept
    {
        if (token.at(0) == '#' && token.size() > 1)
        {
            auto number = ParseNumber(token.substr(1u));

            if (number)
            {
                return {Token::Type::ImmediateInteger, token, line_number, column,
                        static_cast<std::uint32_t>(number.value().get())};
            }

            return {Token::Type::ImmediateInteger, token, line_number, column};
        }

        if (token.at(0) == '/' || token.at(0) == ';')
        {
            return {Token::Type::Comment, token, line_number, column};
        }

        if (phi::optional<phi::i16> number = ParseNumber(token); number.has_value())
        {
            return {Token::Type::IntegerLiteral, token, line_number, column,
                    static_cast<std::uint32_t>(number->get())};
        }

        if (IsFPSR(token))
        {
            return {Token::Type::RegisterStatus, token, line_number, column};
        }

        if (IntRegisterID id = StringToIntRegister(token); id != IntRegisterID::None)
        {
            return {Token::Type::RegisterInt, token, line_number, column,
                    static_cast<std::uint32_t>(id)};
        }

        if (FloatRegisterID id = StringToFloatRegister(token); id != FloatRegisterID::None)
        {
            return {Token::Type::RegisterFloat, token, line_number, column,
                    static_cast<std::uint32_t>(id)};
        }

        if (OpCode opcode = StringToOpCode(token); opcode != OpCode::NONE)
        {
            return {Token::Type::OpCode, token, line_number, column,
                    static_cast<std::uint32_t>(opcode)};
        }

        return {Token::Type::LabelIdentifier, token, line_number, column};
    }

    TokenStream Tokenize(std::string_view source) noexcept
    {
        TokenStream tokens;

        std::string_view current_token;

        phi::u64 current_line_number{1u};
        phi::u64 current_column{1u};
        phi::u64 token_begin{0u};

        phi::boolean parsing_comment{false};

        for (phi::usize i{0u}; i < source.length(); ++i)
        {
            const char c{source.at(i.get())};

            if (c == '\n')
            {
                if (current_token.empty())
                {
                    // Skip empty lines
                    tokens.emplace_back(Token::Type::NewLine, source.substr(i.get(), 1u),
                                        current_line_number, current_column);

                    parsing_comment = false;
                    current_line_number += 1u;
                    current_column = 1u;
                    continue;
                }

                // Otherwise a new line separates tokens
                tokens.emplace_back(
                        ParseToken(source.substr(token_begin.get(), current_token.length()),
                                   current_line_number, current_column - current_token.length()));

                tokens.emplace_back(Token::Type::NewLine, source.substr(i.get(), 1),
                                    current_line_number, current_column);

                current_token   = std::string_view{};
                parsing_comment = false;
                current_line_number += 1u;
                current_column = 0u;
            }
            // Comments begin with an '/' or ';' and after that the entire line is treated as part of the comment
            else if (c == '/' || c == ';')
            {
                if (current_token.empty())
                {
                    token_begin = i;
                }
                else if (!parsing_comment)
                {
                    tokens.emplace_back(ParseToken(
                            source.substr(token_begin.get(), current_token.length()),
                            current_line_number, current_column - current_token.length()));
                    token_begin   = i;
                    current_token = std::string_view{};
                }

                parsing_comment = true;
                current_token   = std::string_view(
                          source.substr(token_begin.get(), current_token.length() + 1));
            }
            else if (parsing_comment)
            {
                // simply append the character
                current_token = std::string_view(
                        source.substr(token_begin.get(), current_token.length() + 1));
            }
            else
            {
                // Not parsing a comment
                switch (c)
                {
                    case ' ':
                    case '\t':
                    case '\v':
                        if (current_token.empty())
                        {
                            current_column += 1u;
                            // We haven't found any usable character for the current token so just skip the whitespace.
                            continue;
                        }

                        // Otherwise a whitespace separates tokens
                        tokens.emplace_back(ParseToken(
                                source.substr(token_begin.get(), current_token.length()),
                                current_line_number, current_column - current_token.length()));
                        current_token = std::string_view{};
                        break;
                    case ':':
                        // Need to parse label names together with their colon
                        if (!current_token.empty())
                        {
                            current_token = std::string_view(
                                    source.substr(token_begin.get(), current_token.length() + 1));
                            tokens.emplace_back(ParseToken(
                                    source.substr(token_begin.get(), current_token.length()),
                                    current_line_number,
                                    current_column + 1u - current_token.length()));

                            current_token = std::string_view{};
                        }
                        else
                        {
                            // Orphan colon
                            token_begin = i;

                            tokens.emplace_back(Token::Type::Colon,
                                                source.substr(token_begin.get(), 1),
                                                current_line_number, current_column);
                        }
                        break;
                    case ',':
                    case '(':
                    case ')':
                        if (!current_token.empty())
                        {
                            tokens.emplace_back(ParseToken(
                                    source.substr(token_begin.get(), current_token.length()),
                                    current_line_number, current_column - current_token.length()));

                            current_token = std::string_view{};
                        }

                        Token::Type type;
                        switch (c)
                        {
                            case ',':
                                type = Token::Type::Comma;
                                break;
                            case '(':
                                type = Token::Type::OpenBracket;
                                break;
                            case ')':
                                type = Token::Type::ClosingBracket;
                                break;
#if !defined(DLXEMU_COVERAGE_BUILD)
                            default:
                                PHI_ASSERT_NOT_REACHED();
                                break;
#endif
                        }

                        token_begin = i;

                        tokens.emplace_back(type, source.substr(token_begin.get(), 1),
                                            current_line_number, current_column);
                        break;

                    default:
                        if (current_token.empty())
                        {
                            token_begin = i;
                        }

                        // simply append the character
                        current_token = std::string_view(
                                source.substr(token_begin.get(), current_token.length() + 1));
                }
            }

            current_column += 1u;
        }

        // Checked the entire string. Parse whats left if anything
        if (!current_token.empty())
        {
            tokens.emplace_back(ParseToken(source.substr(token_begin.get(), current_token.length()),
                                           current_line_number,
                                           current_column - current_token.length()));
        }

        // Finialize token stream
        tokens.finalize();

        return tokens;
    }
} // namespace dlx
