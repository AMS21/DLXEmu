#include "DLX/Parser.hpp"

#include "DLX/InstructionArg.hpp"
#include "DLX/RegisterNames.hpp"
#include <Phi/Config/FunctionLikeMacro.hpp>
#include <Phi/Core/Conversion.hpp>
#include <Phi/Core/Log.hpp>
#include <magic_enum.hpp>
#include <algorithm>
#include <optional>

using namespace phi::literals;

namespace dlx
{
    phi::Boolean CharIsValidForIdentifer(const char c)
    {
        if (std::isalpha(c) || std::isdigit(c))
        {
            return true;
        }

        switch (c)
        {
            case '_':
                return true;

            default:
                return false;
        }
    }

    static phi::Boolean is_integer_literal(std::string_view token)
    {
        if (token.length() == 0)
        {
            return false;
        }

        // First character need to a plus sign a minus sign or a digit but only if theres a digit after that
        if (!std::isdigit(token.at(0)) &&
            !(token.length() > 1 && (token.at(0) == '+' || token.at(0) == '-')))
        {
            return false;
        }

        // Check that the rest of the token is only made up of numbers
        for (phi::usize i{1u}; i < token.length(); ++i)
        {
            if (!std::isdigit(token.at(i.get())))
            {
                return false;
            }
        }

        return true;
    }

    Token ParseToken(std::string_view token, phi::u64 line_number, phi::u64 column)
    {
        if (token.at(0) == '#' && token.size() > 1)
        {
            return Token(Token::Type::ImmediateInteger, token, line_number, column);
        }
        else if (token.at(0) == '/' || token.at(0) == ';')
        {
            return Token(Token::Type::Comment, token, line_number, column);
        }
        else if (is_integer_literal(token))
        {
            return Token(Token::Type::IntegerLiteral, token, line_number, column);
        }

        return Token(Token::Type::Identifier, token, line_number, column);
    }

    std::vector<Token> Parser::Tokenize(std::string_view str)
    {
        std::vector<Token> tokens{};
        tokens.reserve(5);

        std::string current_token;
        current_token.reserve(10);

        phi::u64 current_line_number{1u};
        phi::u64 token_begin{0u};

        phi::Boolean parsing_comment{false};

        for (phi::usize i{0u}; i < str.length(); ++i)
        {
            const char c{str.at(i.get())};

            if (c == '\n')
            {
                if (current_token.empty())
                {
                    tokens.emplace_back(Token::Type::NewLine, str.substr(token_begin.get(), 1),
                                        current_line_number, token_begin);
                    // Skip empty lines
                    continue;
                }

                // Otherwise a new line separates tokens
                tokens.emplace_back(
                        ParseToken(str.substr(token_begin.get(), current_token.length()),
                                   current_line_number, token_begin));

                token_begin = i;

                tokens.emplace_back(Token::Type::NewLine, str.substr(token_begin.get(), 1),
                                    current_line_number, token_begin);

                current_token.clear();
                parsing_comment = false;
                current_line_number += 1u;
            }
            // Comments begin with an '/' or ';' and after that the entire line is treated as part of the comment
            else if (c == '/' || c == ';')
            {
                if (current_token.empty())
                {
                    token_begin = i;
                }

                parsing_comment = true;
                current_token.push_back(c);
            }
            else if (parsing_comment)
            {
                // simply append the character
                current_token.push_back(c);
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
                            // We haven't found any usable character for the current token so just skip the whitespace.
                            continue;
                        }

                        // Otherwise a whitespace separates tokens
                        tokens.push_back(
                                ParseToken(str.substr(token_begin.get(), current_token.length()),
                                           current_line_number, token_begin));
                        current_token.clear();
                        break;
                    case ':':
                        // Need to parse label names together with their colon
                        if (!current_token.empty())
                        {
                            current_token.push_back(c);
                            tokens.emplace_back(ParseToken(
                                    str.substr(token_begin.get(), current_token.length()),
                                    current_line_number, token_begin));

                            current_token.clear();
                        }
                        else
                        {
                            // Orphan colon
                            token_begin = i;

                            tokens.emplace_back(Token::Type::Colon,
                                                str.substr(token_begin.get(), 1),
                                                current_line_number, token_begin);
                            break;
                        }
                        break;
                    case ',':
                    case '(':
                    case ')':
                        if (!current_token.empty())
                        {
                            tokens.emplace_back(ParseToken(
                                    str.substr(token_begin.get(), current_token.length()),
                                    current_line_number, token_begin));

                            current_token.clear();
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
                        }

                        token_begin = i;

                        tokens.emplace_back(type, str.substr(token_begin.get(), 1),
                                            current_line_number, token_begin);
                        break;

                    default:
                        if (current_token.empty())
                        {
                            token_begin = i;
                        }

                        // simply append the character
                        current_token.push_back(c);
                }
            }
        }

        // Checked the entire string. Parse whats left if anything
        if (!current_token.empty())
        {
            tokens.emplace_back(ParseToken(str.substr(token_begin.get(), current_token.length()),
                                           current_line_number, token_begin));
        }

        return tokens;
    }

    static phi::Boolean has_x_more_tokens(const std::vector<Token>& tokens, phi::usize index,
                                          phi::u64 x)
    {
        return index + x <= tokens.size();
    }

    static phi::Boolean has_one_more_token(const std::vector<Token>& tokens, phi::usize index)
    {
        return has_x_more_tokens(tokens, index, 1u);
    }

    static phi::Boolean next_token_is(const std::vector<Token>& tokens, phi::usize index,
                                      Token::Type token_type)
    {
        PHI_ASSERT(has_one_more_token(tokens, index));

        const auto& next_token = tokens.at((index + 1u).get());

        return next_token.GetType() == token_type;
    }

    static phi::Boolean has_one_more_token_of_type(const std::vector<Token>& tokens,
                                                   phi::usize index, Token::Type token_type)
    {
        if (!has_one_more_token(tokens, index))
        {
            return false;
        }

        return (next_token_is(tokens, index, token_type));
    }

    static void AddParseError(ParsedProgram& program, const std::string& message)
    {
        ParseError err;
        err.message = message;

        PHI_LOG_ERROR("Parsing error: {}", message);

        program.m_ParseErrors.emplace_back(err);
    }

    static std::optional<InstructionArg> parse_instruction_argument(
            const Token& token, ArgumentType expected_argument_type,
            const std::vector<Token>& tokens, phi::usize& index, ParsedProgram& program)
    {
        PHI_LOG_INFO("Parsing argument with token '{}' and expected type '{}'", token.DebugInfo(),
                     magic_enum::enum_name(expected_argument_type));

        switch (token.GetType())
        {
            case Token::Type::IntegerLiteral: {
                if (!ArgumentTypeIncludes(expected_argument_type,
                                          ArgumentType::AddressDisplacement))
                {
                    AddParseError(program,
                                  fmt::format("Expected {} but got address displacement",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                // Parse address displacement
                std::int32_t displacement_value{0};
                try
                {
                    displacement_value = std::stoi(token.GetTextString());
                }
                catch (std::invalid_argument& /*e*/)
                {
                    AddParseError(program,
                                  "Failed to parse displacement value for Address displacement");
                    return {};
                }
                catch (std::out_of_range& /*e*/)
                {
                    AddParseError(program, "Value is out of range");
                    return {};
                }

                if (!has_x_more_tokens(tokens, index, 3u))
                {
                    AddParseError(program,
                                  "Not enough arguments left to parse address displacement");
                    return {};
                }

                const Token& first_token  = tokens.at(index.get());
                const Token& second_token = tokens.at((index + 1u).get());
                const Token& third_token  = tokens.at((index + 2u).get());

                if (first_token.GetType() != Token::Type::OpenBracket)
                {
                    AddParseError(program, "Expected open bracket");
                    return {};
                }

                // Second token is the address
                IntRegisterID reg_id = StringToIntRegister(
                        {second_token.GetText().data(), second_token.GetText().size()});

                if (reg_id == IntRegisterID::None)
                {
                    AddParseError(program, "Expected IntRegister");
                    return {};
                }

                if (third_token.GetType() != Token::Type::ClosingBracket)
                {
                    AddParseError(program, "Expected closing bracket");
                    return {};
                }

                index += 3u;

                PHI_LOG_INFO("Parsed address displacement with '{}' displacement and Register '{}'",
                             displacement_value, magic_enum::enum_name(reg_id));

                return ConstructInstructionArgAddressDisplacement(reg_id, displacement_value);
            }
            case Token::Type::Identifier: {
                IntRegisterID reg_id = StringToIntRegister(token.GetTextString());

                if (reg_id != IntRegisterID::None)
                {
                    if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::IntRegister))
                    {
                        AddParseError(program,
                                      fmt::format("Got IntRegister but expected '{}'",
                                                  magic_enum::enum_name(expected_argument_type)));
                        return {};
                    }

                    PHI_LOG_INFO("Parsed identifier as int register {}",
                                 magic_enum::enum_name(reg_id));

                    return ConstructInstructionArgRegisterInt(reg_id);
                }

                // Parse as Label
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::Label))
                {
                    AddParseError(program,
                                  fmt::format("Got Label but expected '{}'",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                if (!std::all_of(token.GetText().begin(), token.GetText().end(),
                                 CharIsValidForIdentifer))
                {
                    AddParseError(program, fmt::format("Invalid label identifier found {}",
                                                       token.GetText()));
                    return {};
                }

                PHI_LOG_INFO("Parsed Label identifier as '{}'", token.GetText());

                return ConstructInstructionArgLabel(token.GetText());
            }
            case Token::Type::ImmediateInteger: {
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::ImmediateInteger))
                {
                    AddParseError(program,
                                  fmt::format("Got ImmediateInteger but expected '{}'",
                                              magic_enum::enum_name(expected_argument_type)));
                    return {};
                }

                std::int32_t value{0};
                try
                {
                    value = std::stoi(token.GetTextString().substr(1));
                }
                catch (std::invalid_argument& /*e*/)
                {
                    AddParseError(program, "Failed to parse immediate Integer value");
                    return {};
                }
                catch (std::out_of_range& /*e*/)
                {
                    AddParseError(program, "Value is out of range");
                    return {};
                }

                PHI_LOG_INFO("Parsed Immediate Integer with value {}", value);

                return ConstructInstructionArgImmediateValue(value);
            }
            default:
                AddParseError(program,
                              fmt::format("Unexpected token of type '{}'", token.GetTypeName()));
                return {};
        }
    }

    static void consume_x_tokens(phi::usize& index, phi::usize x)
    {
        index += x;
    }

    static void consume_current_token(phi::usize& index)
    {
        consume_x_tokens(index, 1u);
    }

    ParsedProgram Parser::Parse(const InstructionLibrary& lib, std::vector<Token>& tokens)
    {
        ParsedProgram program;

        phi::Boolean line_has_instruction{false};

        for (phi::usize index{0u}; index < tokens.size();)
        {
            Token& current_token = tokens.at(index.get());

            consume_current_token(index);

            PHI_LOG_INFO("Parsing '{}'", current_token.DebugInfo());

            switch (current_token.GetType())
            {
                // Ignore comments
                case Token::Type::Comment:
                    PHI_LOG_DEBUG("Ignoring comment");
                    break;
                case Token::Type::NewLine:
                    PHI_LOG_DEBUG("Ignoring newline");
                    line_has_instruction = false;
                    break;
                case Token::Type::Identifier: {
                    if (line_has_instruction)
                    {
                        AddParseError(program, "Expected new line but got identifer");
                        break;
                    }

                    // Handle jump labels
                    // Check if the last character of the identifier is a colon
                    if (current_token.GetText().at(current_token.GetText().size() - 1) == ':')
                    {
                        std::string_view label_name = current_token.GetText().substr(
                                0, current_token.GetText().size() - 1);

                        program.m_JumpData[label_name] = static_cast<std::uint32_t>(program.m_Instructions.size());

                        PHI_LOG_INFO("Added jump label {} -> {}", label_name,
                                     program.m_Instructions.size());
                        break;
                    }

                    // Handle normal instructions
                    // First we need to parse the instruction itself so we know how many arguments it expects
                    OpCode opcode = StringToOpCode(current_token.GetTextString());

                    if (opcode == OpCode::NONE)
                    {
                        AddParseError(program,
                                      fmt::format("Failed to parse instruction '{}' not found.",
                                                  current_token.GetText()));
                        break;
                    }

                    PHI_LOG_INFO("Instruction opcode: {}", magic_enum::enum_name(opcode));

                    const InstructionInfo& info = lib.LookUp(opcode);

                    // Make sure we got no problems here
                    PHI_ASSERT(info.GetArgumentType(0_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetArgumentType(0_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetArgumentType(0_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetExecutor());

                    phi::u8 number_of_argument_required = info.GetNumberOfRequiredArguments();
                    PHI_LOG_INFO("Instruction requires {} arguments",
                                 number_of_argument_required.get());

                    // Create instruction
                    Instruction instruction(info);

                    // Parse arguments
                    for (phi::u8 argument_num{0_u8}; argument_num < number_of_argument_required;)
                    {
                        // Get next token
                        if (!has_one_more_token(tokens, index))
                        {
                            AddParseError(
                                    program,
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
                            PHI_LOG_DEBUG("Skipping comma");
                            continue;
                        }

                        if (current_token.GetType() == Token::Type::NewLine)
                        {
                            AddParseError(program, "Unexpected end of line");
                            break;
                        }

                        std::optional<InstructionArg> optional_parsed_argument =
                                parse_instruction_argument(current_token,
                                                           info.GetArgumentType(argument_num),
                                                           tokens, index, program);
                        if (!optional_parsed_argument.has_value())
                        {
                            AddParseError(program, fmt::format("Failed to parse argument number {}",
                                                               argument_num.get()));
                            break;
                        }

                        // Successfully parsed one argument
                        InstructionArg parsed_argument = optional_parsed_argument.value();

                        instruction.SetArgument(argument_num, parsed_argument);
                        argument_num++;

                        PHI_LOG_INFO("Successfully parsed argument {}", argument_num.get());
                    }

                    // Only add the instruction if we got no parsing errors
                    if (program.m_ParseErrors.empty())
                    {
                        PHI_LOG_INFO("Successfully parsed instruction '{}'",
                                     instruction.DebugInfo());
                        program.m_Instructions.emplace_back(std::move(instruction));
                        line_has_instruction = true;
                    }
                    break;
                }

                default:
                    AddParseError(program, "Unexpected token");
                    break;
            }
        }

        return program;
    }

    ParsedProgram Parser::Parse(const InstructionLibrary& lib, std::string_view source)
    {
        std::vector<Token> tokens = Tokenize(source);
        return Parse(lib, tokens);
    }
} // namespace dlx
