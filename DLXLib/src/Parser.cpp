#include "DLX/Parser.hpp"

#include "DLX/Instruction.hpp"
#include "DLX/InstructionArgument.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/InstructionLibrary.hpp"
#include "DLX/Logger.hpp"
#include "DLX/OpCode.hpp"
#include "DLX/ParseError.hpp"
#include "DLX/ParserUtils.hpp"
#include "DLX/RegisterNames.hpp"
#include "DLX/Token.hpp"
#include "DLX/TokenStream.hpp"
#include "DLX/Tokenize.hpp"
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/conversion.hpp>
#include <phi/core/optional.hpp>
#include <phi/core/types.hpp>
#include <phi/preprocessor/function_like_macro.hpp>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string_view>

using namespace phi::literals;

namespace dlx
{
    static phi::optional<InstructionArgument> parse_instruction_argument(
            const Token& token, ArgumentType expected_argument_type, TokenStream& tokens,
            ParsedProgram& program) noexcept
    {
        // DLX_INFO("Parsing argument with token '{}' and expected type '{}'", token.DebugInfo(),
        //              dlx::enum_name(expected_argument_type));

        switch (token.GetType())
        {
            case Token::Type::IntegerLiteral: {
                if (!ArgumentTypeIncludes(expected_argument_type,
                                          ArgumentType::AddressDisplacement))
                {
                    program.AddParseError(ConstructUnexpectedArgumentTypeParseError(
                            token, expected_argument_type, ArgumentType::AddressDisplacement));

                    return {};
                }

                // Get address displacement
                PHI_ASSERT(token.HasHint());

                std::int16_t value = static_cast<std::int16_t>(token.GetHint());

                if (!tokens.has_x_more(3u))
                {
                    program.AddParseError(
                            ConstructTooFewArgumentsAddressDisplacementParseError(token));
                    return {};
                }

                auto         it           = tokens.current_position();
                const Token& first_token  = tokens.consume();
                const Token& second_token = tokens.consume();
                const Token& third_token  = tokens.consume();

                if (first_token.GetType() != Token::Type::OpenBracket)
                {
                    program.AddParseError(ConstructUnexpectedTokenParseError(
                            first_token, Token::Type::OpenBracket));
                    return {};
                }

                // Second token is the register
                if (second_token.GetType() != Token::Type::RegisterInt)
                {
                    program.AddParseError(ConstructUnexpectedTokenParseError(
                            second_token, Token::Type::RegisterInt));
                    return {};
                }

                if (third_token.GetType() != Token::Type::ClosingBracket)
                {
                    program.AddParseError(ConstructUnexpectedTokenParseError(
                            third_token, Token::Type::ClosingBracket));
                    return {};
                }

                // Consume the 3 tokens
                tokens.set_position(it + 3u);

                //DLX_INFO("Parsed address displacement with '{}' displacement and Register '{}'",
                //             value, dlx::enum_name(reg_id));

                return ConstructInstructionArgumentAddressDisplacement(
                        static_cast<IntRegisterID>(second_token.GetHint()), value);
            }
            case Token::Type::RegisterInt: {
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::IntRegister))
                {
                    program.AddParseError(ConstructUnexpectedArgumentTypeParseError(
                            token, expected_argument_type, ArgumentType::IntRegister));
                    return {};
                }

                //DLX_INFO("Parsed identifier as int register {}",
                //             dlx::enum_name(reg_id));

                return ConstructInstructionArgumentRegisterInt(
                        static_cast<IntRegisterID>(token.GetHint()));
            }
            case Token::Type::RegisterFloat: {
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::FloatRegister))
                {
                    program.AddParseError(ConstructUnexpectedArgumentTypeParseError(
                            token, expected_argument_type, ArgumentType::FloatRegister));
                    return {};
                }

                //DLX_INFO("Parsed identifier as float register {}",
                //             dlx::enum_name(float_reg_id));

                return ConstructInstructionArgumentRegisterFloat(
                        static_cast<FloatRegisterID>(token.GetHint()));
            }
            case Token::Type::RegisterStatus: {
                program.AddParseError(ConstructReservedIdentiferParseError(token));

                return {};
            }
            case Token::Type::LabelIdentifier: {
                // Parse as Label
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::Label))
                {
                    program.AddParseError(ConstructUnexpectedArgumentTypeParseError(
                            token, expected_argument_type, ArgumentType::Label));
                    return {};
                }

                if (IsReservedIdentifier(token.GetText()))
                {
                    program.AddParseError(ConstructReservedIdentiferParseError(token));
                    return {};
                }

                if (!IsValidIdentifier(token.GetText()))
                {
                    program.AddParseError(ConstructInvalidLabelIdentifierParseError(token));
                    return {};
                }

                //DLX_INFO("Parsed Label identifier as '{}'", token.GetText());

                return ConstructInstructionArgumentLabel(token.GetText());
            }
            case Token::Type::ImmediateInteger: {
                if (!ArgumentTypeIncludes(expected_argument_type, ArgumentType::ImmediateInteger))
                {
                    program.AddParseError(ConstructUnexpectedArgumentTypeParseError(
                            token, expected_argument_type, ArgumentType::ImmediateInteger));
                    return {};
                }

                if (token.HasHint())
                {
                    return ConstructInstructionArgumentImmediateValue(
                            static_cast<std::int16_t>(token.GetHint()));
                }

                auto parsed_value = ParseNumber(token.GetText().substr(1));
                if (!parsed_value)
                {
                    program.AddParseError(ConstructInvalidNumberParseError(token));
                    return {};
                }

                //DLX_INFO("Parsed Immediate Integer with value {}", parsed_value.value().unsafe());

#if !defined(DLXEMU_COVERAGE_BUILD)
                return ConstructInstructionArgumentImmediateValue(parsed_value.value().unsafe());
#endif
            }

            default:
                program.AddParseError(
                        ConstructUnexpectedTokenParseError(token, Token::Type::Unknown));
                return {};
        }
    }

    ParsedProgram Parser::Parse(TokenStream& tokens) noexcept
    {
        ParsedProgram program;

        program.m_Tokens = tokens;

        phi::boolean line_has_instruction{false};
        phi::usize   label_count{0u};

        while (tokens.has_more())
        {
            const Token& current_token = tokens.consume();

            //DLX_INFO("Parsing '{}'", current_token.DebugInfo());

            switch (current_token.GetType())
            {
                // Ignore comments
                case Token::Type::Comment:
                    //DLX_DEBUG("Ignoring comment");
                    break;

                case Token::Type::NewLine:
                    //DLX_DEBUG("Ignoring newline");
                    line_has_instruction = false;
                    break;

                case Token::Type::LabelIdentifier: {
                    if (line_has_instruction)
                    {
                        program.AddParseError(ConstructUnexpectedTokenParseError(
                                current_token, Token::Type::NewLine));
                        break;
                    }

                    // Handle jump labels
                    // Check if the last character of the identifier is a colon
                    if (current_token.GetText().back() != ':')
                    {
                        program.AddParseError(
                                ConstructInvalidLabelIdentifierParseError(current_token));
                        break;
                    }

                    std::string_view label_name = current_token.GetText();
                    label_name.remove_suffix(1);

                    if (IsReservedIdentifier(label_name))
                    {
                        program.AddParseError(ConstructReservedIdentiferParseError(
                                current_token.GetLineNumber().unsafe(),
                                current_token.GetColumn().unsafe(), label_name));
                        break;
                    }

                    if (!IsValidIdentifier(label_name))
                    {
                        program.AddParseError(
                                ConstructInvalidLabelIdentifierParseError(current_token));
                        break;
                    }

                    // Check if label was already defined
                    if (program.m_JumpData.find(label_name) != program.m_JumpData.end())
                    {
                        // Find first defintions of label
                        const Token* first_label_definition =
                                tokens.find_first_token_if([&](const Token& t) {
                                    if (t.GetType() == Token::Type::LabelIdentifier)
                                    {
                                        std::string_view token_label_name = t.GetText();
                                        token_label_name.remove_suffix(1);

                                        if (token_label_name == label_name)
                                        {
                                            return true;
                                        }
                                    }

                                    return false;
                                });

                        PHI_ASSERT(first_label_definition);

                        program.AddParseError(ConstructLabelAlreadyDefinedParseError(
                                current_token, *first_label_definition));
                        break;
                    }

                    program.m_JumpData[label_name] =
                            static_cast<std::uint32_t>(program.m_Instructions.size());
                    label_count += 1u;

                    //DLX_INFO("Added jump label {} -> {}", label_name,
                    //             program.m_Instructions.size());

                    break;
                }

                case Token::Type::OpCode: {
                    if (line_has_instruction)
                    {
                        program.AddParseError(
                                ConstructOneInstructionPerLineParseError(current_token));
                        break;
                    }

                    label_count = 0u;

                    // Handle normal instructions
                    PHI_ASSERT(current_token.HasHint());
                    OpCode opcode = static_cast<OpCode>(current_token.GetHint());

                    //DLX_INFO("Instruction opcode: {}", dlx::enum_name(opcode));

                    const InstructionInfo& info = LookUpIntructionInfo(opcode);

                    // Make sure we got no problems here
                    PHI_ASSERT(info.GetArgumentType(0_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetArgumentType(1_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetArgumentType(2_u8) != ArgumentType::Unknown);
                    PHI_ASSERT(info.GetExecutor());

                    const phi::u8 number_of_argument_required = info.GetNumberOfRequiredArguments();
                    //DLX_INFO("Instruction requires {} arguments",
                    //             number_of_argument_required.unsafe());

                    // Create instruction
                    Instruction  instruction(info, current_token.GetLineNumber());
                    phi::boolean consumed_comma{false};

                    // Parse arguments
                    for (phi::u8 argument_num{0_u8}; argument_num < number_of_argument_required;)
                    {
                        // Get next token
                        if (!tokens.has_more())
                        {
                            program.AddParseError(ConstructTooFewArgumentsParseError(
                                    current_token, number_of_argument_required.unsafe(),
                                    argument_num.unsafe()));
                            break;
                        }

                        const Token& token = tokens.consume();

                        // Skip commas
                        if (token.GetType() == Token::Type::Comma)
                        {
                            if (consumed_comma)
                            {
                                program.AddParseError(ConstructTooManyCommaParseError(token));
                            }

                            consumed_comma = true;
                            //DLX_DEBUG("Skipping comma");
                            continue;
                        }

                        if (token.GetType() == Token::Type::NewLine)
                        {
                            program.AddParseError(ConstructTooFewArgumentsParseError(
                                    token, number_of_argument_required.unsafe(),
                                    argument_num.unsafe()));
                            break;
                        }

                        phi::optional<InstructionArgument> optional_parsed_argument =
                                parse_instruction_argument(
                                        token, info.GetArgumentType(argument_num), tokens, program);
                        if (!optional_parsed_argument.has_value())
                        {
                            // The parse_instruction_argument function should already have added a parse error with more detail
                            break;
                        }

                        // Successfully parsed one argument
                        InstructionArgument parsed_argument = optional_parsed_argument.value();

                        instruction.SetArgument(argument_num, parsed_argument);
                        argument_num++;
                        consumed_comma = false;

                        //DLX_INFO("Successfully parsed argument {}", argument_num.unsafe());
                    }

                    //DLX_INFO("Successfully parsed instruction '{}'",
                    //            instruction.DebugInfo());
                    program.m_Instructions.emplace_back(instruction);
                    line_has_instruction = true;
                    break;
                }

                default:
                    Token::Type expected_token_type = Token::Type::Unknown;
                    if (line_has_instruction)
                    {
                        expected_token_type = Token::Type::NewLine;
                    }

                    program.AddParseError(
                            ConstructUnexpectedTokenParseError(current_token, expected_token_type));
                    break;
            }
        }

        // Check for empty labels
        if (label_count > 0u)
        {
            for (auto it = tokens.rbegin(); label_count > 0u; ++it)
            {
                PHI_ASSERT(it != tokens.rend(), "Iterator should never reach the end");

                const Token& token = *it;
                if (token.GetType() == Token::Type::LabelIdentifier)
                {
                    program.AddParseError(ConstructEmptyLabelParseError(token));
                    --label_count;
                }
            }
        }

        return program;
    }

    ParsedProgram Parser::Parse(std::string_view source) noexcept
    {
        TokenStream tokens = Tokenize(source);
        return Parse(tokens);
    }
} // namespace dlx
