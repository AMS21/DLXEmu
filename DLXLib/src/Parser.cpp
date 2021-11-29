#include "DLX/Parser.hpp"

#include "DLX/Instruction.hpp"
#include "DLX/InstructionArgument.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/InstructionLibrary.hpp"
#include "DLX/OpCode.hpp"
#include "DLX/ParseError.hpp"
#include "DLX/ParserUtils.hpp"
#include "DLX/RegisterNames.hpp"
#include "DLX/Token.hpp"
#include "DLX/TokenStream.hpp"
#include "DLX/Tokenize.hpp"
#include <Phi/Config/FunctionLikeMacro.hpp>
#include <Phi/Core/Assert.hpp>
#include <Phi/Core/Boolean.hpp>
#include <Phi/Core/Conversion.hpp>
#include <Phi/Core/Log.hpp>
#include <Phi/Core/Types.hpp>
#include <magic_enum.hpp>
#include <algorithm>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string_view>

using namespace phi::literals;

namespace dlx
{
    static std::optional<InstructionArgument> parse_instruction_argument(
            const Token& token, ArgumentType expected_argument_type, TokenStream& tokens,
            ParsedProgram& program) noexcept
    {
        // PHI_LOG_INFO("Parsing argument with token '{}' and expected type '{}'", token.DebugInfo(),
        //              magic_enum::enum_name(expected_argument_type));

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

                // Parse address displacement
                auto displacement_value = ParseNumber(token.GetText());
                if (!displacement_value)
                {
                    program.AddParseError(ConstructInvalidNumberParseError(token));
                    return {};
                }
                std::int16_t value = displacement_value.value().get();

                if (!tokens.has_x_more(2u))
                {
                    program.AddParseError(
                            ConstructTooFewArgumentsAddressDisplacementParseError(token));
                    return {};
                }

                auto         it           = tokens.current_position();
                const Token& first_token  = *it++;
                const Token& second_token = *it++;
                const Token& third_token  = *it++;

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
                tokens.set_position(it);

                //PHI_LOG_INFO("Parsed address displacement with '{}' displacement and Register '{}'",
                //             value, magic_enum::enum_name(reg_id));

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

                //PHI_LOG_INFO("Parsed identifier as int register {}",
                //             magic_enum::enum_name(reg_id));

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

                //PHI_LOG_INFO("Parsed identifier as float register {}",
                //             magic_enum::enum_name(float_reg_id));

                return ConstructInstructionArgumentRegisterFloat(
                        static_cast<FloatRegisterID>(token.GetHint()));
            }
            case Token::Type::RegisterStatus: {
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

                //PHI_LOG_INFO("Parsed Label identifier as '{}'", token.GetText());

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

                //PHI_LOG_INFO("Parsed Immediate Integer with value {}", parsed_value.value().get());

#if !defined(DLXEMU_COVERAGE_BUILD)
                return ConstructInstructionArgumentImmediateValue(parsed_value.value().get());
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

        phi::Boolean line_has_instruction{false};
        phi::usize   label_count{0u};

        while (tokens.has_more())
        {
            const Token& current_token = tokens.consume();

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
                        program.AddParseError(ConstructUnexpectedTokenParseError(
                                current_token, Token::Type::NewLine));
                        break;
                    }

                    // Handle jump labels
                    // Check if the last character of the identifier is a colon
                    if (current_token.GetText().at(current_token.GetText().size() - 1) != ':')
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
                                current_token.GetLineNumber().get(),
                                current_token.GetColumn().get(), label_name));
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

                    //PHI_LOG_INFO("Added jump label {} -> {}", label_name,
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
                    OpCode opcode = static_cast<OpCode>(current_token.GetHint());

                    //PHI_LOG_INFO("Instruction opcode: {}", magic_enum::enum_name(opcode));

                    const InstructionInfo& info = LookUpIntructionInfo(opcode);

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
                        if (!tokens.has_more())
                        {
                            program.AddParseError(ConstructTooFewArgumentsParseError(
                                    current_token, number_of_argument_required.get(),
                                    argument_num.get()));
                            break;
                        }

                        const Token& token = tokens.consume();

                        // Skip commas
                        if (token.GetType() == Token::Type::Comma)
                        {
                            //PHI_LOG_DEBUG("Skipping comma");
                            continue;
                        }

                        if (token.GetType() == Token::Type::NewLine)
                        {
                            phi::u8 missing_arguments = number_of_argument_required - argument_num;
                            program.AddParseError(ConstructTooFewArgumentsParseError(
                                    token, number_of_argument_required.get(), argument_num.get()));
                            break;
                        }

                        std::optional<InstructionArgument> optional_parsed_argument =
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
                    program.AddParseError(ConstructUnexpectedTokenParseError(current_token,
                                                                             Token::Type::Unknown));
                    break;
            }
        }

        // Check for empty labels
        if (label_count > 0u)
        {
            for (auto it = --tokens.end(); label_count > 0u; --it)
            {
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
