#include "DLX/Parser.hpp"

#include "DLX/Instruction.hpp"
#include "DLX/InstructionArg.hpp"
#include "DLX/OpCode.hpp"
#include "DLX/ParserUtils.hpp"
#include "DLX/RegisterNames.hpp"
#include "DLX/Token.hpp"
#include <Phi/Config/FunctionLikeMacro.hpp>
#include <Phi/Core/Assert.hpp>
#include <Phi/Core/Boolean.hpp>
#include <Phi/Core/Conversion.hpp>
#include <Phi/Core/Log.hpp>
#include <Phi/Core/Types.hpp>
#include <magic_enum.hpp>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/fmt.h>
#include <algorithm>
#include <limits>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string_view>

using namespace phi::literals;

namespace dlx
{
    Token ParseToken(std::string_view token, phi::u64 line_number, phi::u64 column) noexcept
    {
        if (token.at(0) == '#' && token.size() > 1)
        {
            auto number = ParseNumber(token.substr(1u));

            if (number)
            {
                //PHI_LOG_TRACE("Parsed number: {:d}", number.value().get());
                return Token(Token::Type::ImmediateInteger, token, line_number, column,
                             number.value().get());
            }

            return Token(Token::Type::ImmediateInteger, token, line_number, column);
        }

        if (token.at(0) == '/' || token.at(0) == ';')
        {
            return Token(Token::Type::Comment, token, line_number, column);
        }

        if (auto number = ParseNumber(token); number.has_value())
        {
            return Token(Token::Type::IntegerLiteral, token, line_number, column, number->get());
        }

        std::string token_upper(token.data(), token.size());
        std::transform(token_upper.begin(), token_upper.end(), token_upper.begin(), ::toupper);

        if (token_upper == "FPSR")
        {
            return Token(Token::Type::RegisterStatus, token, line_number, column);
        }

        if (IntRegisterID id = StringToIntRegister(token_upper); id != IntRegisterID::None)
        {
            return Token(Token::Type::RegisterInt, token, line_number, column,
                         static_cast<std::uint32_t>(id));
        }

        if (FloatRegisterID id = StringToFloatRegister(token_upper); id != FloatRegisterID::None)
        {
            return Token(Token::Type::RegisterFloat, token, line_number, column,
                         static_cast<std::uint32_t>(id));
        }

        if (OpCode opcode = StringToOpCode(token_upper); opcode != OpCode::NONE)
        {
            return Token(Token::Type::OpCode, token, line_number, column,
                         static_cast<std::uint32_t>(opcode));
        }

        return Token(Token::Type::LabelIdentifier, token, line_number, column);
    }

    std::vector<Token> Parser::Tokenize(std::string_view source) noexcept
    {
        std::vector<Token> tokens{};
        tokens.reserve(5);

        std::string_view current_token;

        phi::u64 current_line_number{1u};
        phi::u64 current_column{1u};
        phi::u64 token_begin{0u};

        phi::Boolean parsing_comment{false};

        for (phi::usize i{0u}; i < source.length(); ++i)
        {
            const char c{source.at(i.get())};

            if (c == '\n')
            {
                if (current_token.empty())
                {
                    // Skip empty lines
                    tokens.emplace_back(Token::Type::NewLine, source.substr(token_begin.get(), 1),
                                        current_line_number, current_column - 1u);

                    parsing_comment = false;
                    current_line_number += 1u;
                    current_column = 1u;
                    continue;
                }

                // Otherwise a new line separates tokens
                tokens.emplace_back(
                        ParseToken(source.substr(token_begin.get(), current_token.length()),
                                   current_line_number, current_column - current_token.length()));

                token_begin = i;

                tokens.emplace_back(Token::Type::NewLine, source.substr(token_begin.get(), 1),
                                    current_line_number, current_column - 1u);

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
                            default:
                                PHI_ASSERT_NOT_REACHED();
                                break;
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

        return tokens;
    }

    static phi::Boolean has_x_more_tokens(const std::vector<Token>& tokens, phi::usize index,
                                          phi::u64 x) noexcept
    {
        return index + x <= tokens.size();
    }

    static phi::Boolean has_one_more_token(const std::vector<Token>& tokens,
                                           phi::usize                index) noexcept
    {
        return has_x_more_tokens(tokens, index, 1u);
    }

    static phi::Boolean next_token_is(const std::vector<Token>& tokens, phi::usize index,
                                      Token::Type token_type) noexcept
    {
        PHI_ASSERT(has_one_more_token(tokens, index));

        const auto& next_token = tokens.at((index + 1u).get());

        return next_token.GetType() == token_type;
    }

    static phi::Boolean has_one_more_token_of_type(const std::vector<Token>& tokens,
                                                   phi::usize                index,
                                                   Token::Type               token_type) noexcept
    {
        if (!has_one_more_token(tokens, index))
        {
            return false;
        }

        return (next_token_is(tokens, index, token_type));
    }

    static const Token* find_first_token_of_type(const std::vector<Token>& tokens,
                                                                const Token::Type type) noexcept
    {
        for (const Token& token : tokens)
        {
            if (token.GetType() == type)
            {
                return &token;
            }
        }

        return nullptr;
    }

    static const Token* find_last_token_of_type(const std::vector<Token>& tokens, const Token::Type type) noexcept
    {
        for (auto it = tokens.rbegin(); it != tokens.rend();++it)
        {
            if (it->GetType() == type)
            {
                return &(*it);
            }
        }

        return nullptr;
    }

    static void AddParseError(ParsedProgram& program, const Token& current_token,
                              const std::string& message) noexcept
    {
        ParseError err;
        err.line_number = current_token.GetLineNumber().get();
        err.column      = current_token.GetColumn().get();
        err.message     = message;

        //PHI_LOG_ERROR("Parsing error: {}", message);

        program.m_ParseErrors.emplace_back(err);
    }

    static std::optional<InstructionArg> parse_instruction_argument(
            const Token& token, ArgumentType expected_argument_type,
            const std::vector<Token>& tokens, phi::usize& index, ParsedProgram& program) noexcept
    {
        // PHI_LOG_INFO("Parsing argument with token '{}' and expected type '{}'", token.DebugInfo(),
        //              magic_enum::enum_name(expected_argument_type));

        switch (token.GetType())
        {
            case Token::Type::IntegerLiteral: {
                if (!ArgumentTypeIncludes(expected_argument_type,
                                          ArgumentType::AddressDisplacement))
                {
                    AddParseError(program, token,
                                  fmt::format("Expected {} but got address displacement",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                // Parse address displacement
                auto displacement_value = ParseNumber(token.GetText());
                if (!displacement_value)
                {
                    AddParseError(program, token,
                                  "Failed to parse displacement value for Address displacement");
                    return {};
                }
                std::int16_t value = displacement_value.value().get();

                if (!has_x_more_tokens(tokens, index, 3u))
                {
                    AddParseError(program, token,
                                  "Not enough arguments left to parse address displacement");
                    return {};
                }

                const Token& first_token  = tokens.at(index.get());
                const Token& second_token = tokens.at((index + 1u).get());
                const Token& third_token  = tokens.at((index + 2u).get());

                if (first_token.GetType() != Token::Type::OpenBracket)
                {
                    AddParseError(program, token, "Expected open bracket");
                    return {};
                }

                // Second token is the register
                if (second_token.GetType() != Token::Type::RegisterInt)
                {
                    AddParseError(program, token, "Expected IntRegister");
                    return {};
                }

                if (third_token.GetType() != Token::Type::ClosingBracket)
                {
                    AddParseError(program, token, "Expected closing bracket");
                    return {};
                }

                index += 3u;

                //PHI_LOG_INFO("Parsed address displacement with '{}' displacement and Register '{}'",
                //             value, magic_enum::enum_name(reg_id));

                return ConstructInstructionArgAddressDisplacement(
                        static_cast<IntRegisterID>(second_token.GetHint()), value);
            }
            case Token::Type::RegisterInt: {
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::IntRegister))
                {
                    AddParseError(program, token,
                                  fmt::format("Got IntRegister but expected '{}'",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                //PHI_LOG_INFO("Parsed identifier as int register {}",
                //             magic_enum::enum_name(reg_id));

                return ConstructInstructionArgRegisterInt(
                        static_cast<IntRegisterID>(token.GetHint()));
            }
            case Token::Type::RegisterFloat: {
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::FloatRegister))
                {
                    AddParseError(program, token,
                                  fmt::format("Got FloatRegister but expected '{}'",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                //PHI_LOG_INFO("Parsed identifier as float register {}",
                //             magic_enum::enum_name(float_reg_id));

                return ConstructInstructionArgRegisterFloat(
                        static_cast<FloatRegisterID>(token.GetHint()));
            }
            case Token::Type::RegisterStatus: {
            }
            case Token::Type::LabelIdentifier: {
                // Parse as Label
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::Label))
                {
                    AddParseError(program, token,
                                  fmt::format("Got Label but expected '{}'",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                if (IsReservedIdentifier(token.GetText()))
                {
                    AddParseError(
                            program, token,
                            fmt::format("Cannot used reserved identifier {}", token.GetText()));
                    return {};
                }

                if (!IsValidIdentifier(token.GetText()))
                {
                    AddParseError(
                            program, token,
                            fmt::format("Invalid label identifier found {}", token.GetText()));
                    return {};
                }

                //PHI_LOG_INFO("Parsed Label identifier as '{}'", token.GetText());

                return ConstructInstructionArgLabel(token.GetText());
            }
            case Token::Type::ImmediateInteger: {
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::ImmediateInteger))
                {
                    AddParseError(program, token,
                                  fmt::format("Got ImmediateInteger but expected '{}'",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                if (token.HasHint())
                {
                    return ConstructInstructionArgImmediateValue(
                            static_cast<std::int16_t>(token.GetHint()));
                }

                auto parsed_value = ParseNumber(token.GetText().substr(1));
                if (!parsed_value)
                {
                    AddParseError(program, token, "Failed to parse immediate Integer value");
                    return {};
                }

                //PHI_LOG_INFO("Parsed Immediate Integer with value {}", parsed_value.value().get());

                return ConstructInstructionArgImmediateValue(parsed_value.value().get());
            }
            default:
                AddParseError(program, token,
                              fmt::format("Unexpected token of type '{}'", token.GetTypeName()));
                return {};
        }
    }

    static void consume_x_tokens(phi::usize& index, phi::usize x) noexcept
    {
        index += x;
    }

    static void consume_current_token(phi::usize& index) noexcept
    {
        consume_x_tokens(index, 1u);
    }

    ParsedProgram Parser::Parse(const InstructionLibrary& lib, std::vector<Token>& tokens) noexcept
    {
        ParsedProgram program;

        program.m_Tokens = tokens;

        phi::Boolean line_has_instruction{false};
        phi::Boolean last_line_was_label{false};

        for (phi::usize index{0u}; index < tokens.size();)
        {
            Token& current_token = tokens.at(index.get());

            consume_current_token(index);

            //PHI_LOG_INFO("Parsing '{}'", current_token.DebugInfo());

            switch (current_token.GetType())
            {
                // Ignore comments
                case Token::Type::Comment:
                    //PHI_LOG_DEBUG("Ignoring comment");
                    break;
                case Token::Type::NewLine:
                    //PHI_LOG_DEBUG("Ignoring newline");
                    line_has_instruction = false;
                    break;
                case Token::Type::LabelIdentifier: {
                    if (line_has_instruction)
                    {
                        AddParseError(program, current_token,
                                      "Expected new line but got label identifer");
                        break;
                    }

                    // Handle jump labels
                    // Check if the last character of the identifier is a colon
                    if (current_token.GetText().at(current_token.GetText().size() - 1) != ':')
                    {
                        AddParseError(program, current_token,
                                      "Label identifier is missing a colon");
                        break;
                    }

                    std::string_view label_name =
                            current_token.GetText().substr(0, current_token.GetText().size() - 1);

                    if (IsReservedIdentifier(label_name))
                    {
                        AddParseError(
                                program, current_token,
                                fmt::format("Cannot used reserved identifier {} as jump label",
                                            label_name));
                        break;
                    }

                    // Check if label was already defined
                    if (program.m_JumpData.find(label_name) != program.m_JumpData.end())
                    {
                        AddParseError(program, current_token,
                                      fmt::format("Label '{:s}' already defined", label_name));
                        break;
                    }

                    program.m_JumpData[label_name] =
                            static_cast<std::uint32_t>(program.m_Instructions.size());
                    last_line_was_label = true;

                    //PHI_LOG_INFO("Added jump label {} -> {}", label_name,
                    //             program.m_Instructions.size());

                    break;
                }
                case Token::Type::OpCode: {
                    if (line_has_instruction)
                    {
                        AddParseError(program, current_token, "Expected new line but got op code");
                        break;
                    }

                    last_line_was_label = false;

                    // Handle normal instructions
                    OpCode opcode = static_cast<OpCode>(current_token.GetHint());

                    //PHI_LOG_INFO("Instruction opcode: {}", magic_enum::enum_name(opcode));

                    const InstructionInfo& info = lib.LookUp(opcode);

                    // Make sure we got no problems here
                    PHI_ASSERT(info.GetArgumentType(0_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetArgumentType(0_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetArgumentType(0_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetExecutor());

                    phi::u8 number_of_argument_required = info.GetNumberOfRequiredArguments();
                    //PHI_LOG_INFO("Instruction requires {} arguments",
                    //             number_of_argument_required.get());

                    // Create instruction
                    Instruction instruction(info);

                    // Parse arguments
                    for (phi::u8 argument_num{0_u8}; argument_num < number_of_argument_required;)
                    {
                        // Get next token
                        if (!has_one_more_token(tokens, index))
                        {
                            AddParseError(
                                    program, current_token,
                                    fmt::format("Missing {} arguments for instruction {}",
                                                (number_of_argument_required - argument_num).get(),
                                                magic_enum::enum_name(opcode)));
                            break;
                        }

                        current_token = tokens.at(index.get());
                        consume_current_token(index);

                        // Skip commas
                        if (current_token.GetType() == Token::Type::Comma)
                        {
                            //PHI_LOG_DEBUG("Skipping comma");
                            continue;
                        }

                        if (current_token.GetType() == Token::Type::NewLine)
                        {
                            AddParseError(program, current_token, "Unexpected end of line");
                            break;
                        }

                        std::optional<InstructionArg> optional_parsed_argument =
                                parse_instruction_argument(current_token,
                                                           info.GetArgumentType(argument_num),
                                                           tokens, index, program);
                        if (!optional_parsed_argument.has_value())
                        {
                            AddParseError(program, current_token,
                                          fmt::format("Failed to parse argument number {}",
                                                      argument_num.get()));
                            break;
                        }

                        // Successfully parsed one argument
                        InstructionArg parsed_argument = optional_parsed_argument.value();

                        instruction.SetArgument(argument_num, parsed_argument);
                        argument_num++;

                        //PHI_LOG_INFO("Successfully parsed argument {}", argument_num.get());
                    }

                    // Only add the instruction if we got no parsing errors
                    if (program.m_ParseErrors.empty())
                    {
                        //PHI_LOG_INFO("Successfully parsed instruction '{}'",
                        //            instruction.DebugInfo());
                        program.m_Instructions.emplace_back(instruction);
                        line_has_instruction = true;
                    }
                    break;
                }

                default:
                    AddParseError(program, current_token, "Unexpected token");
                    break;
            }
        }

        if (last_line_was_label)
        {
            auto optional_token = find_last_token_of_type(tokens, Token::Type::LabelIdentifier);
            PHI_ASSERT(optional_token);

            AddParseError(program, *optional_token, "Cannot have empty jump labels");
        }

        return program;
    }

    ParsedProgram Parser::Parse(const InstructionLibrary& lib, std::string_view source) noexcept
    {
        std::vector<Token> tokens = Tokenize(source);
        return Parse(lib, tokens);
    }
} // namespace dlx
