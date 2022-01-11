#include "DLX/ParseError.hpp"

#include "DLX/InstructionInfo.hpp"
#include <Phi/Core/Assert.hpp>
#include <magic_enum.hpp>
#include <spdlog/fmt/bundled/core.h>

namespace dlx
{
    ParseError::ParseError() noexcept
    {}

    ParseError::Type ParseError::GetType() const noexcept
    {
        return m_Type;
    }

    std::uint32_t ParseError::GetLineNumber() const noexcept
    {
        return m_LineNumber;
    }

    std::uint32_t ParseError::GetColumn() const noexcept
    {
        return m_Column;
    }

    std::string ParseError::ConstructMessage() const noexcept
    {
        switch (m_Type)
        {
            case Type::UnexpectedArgumentType: {
                const UnexpectedArgumentType& detail = GetUnexpectedArgumentType();

                return fmt::format("Expected {:s} but got {:s}",
                                   magic_enum::enum_name(detail.expected_type),
                                   magic_enum::enum_name(detail.actual_type));
            }

            case Type::InvalidNumber: {
                const InvalidNumber& detail = GetInvalidNumber();

                return fmt::format("'{:s}' is not a valid number", detail.text);
            }

            case Type::TooFewArgumentsAddressDisplacement: {
                return fmt::format("Not enough arguments left to parse address displacement");
            }

            case Type::UnexpectedToken: {
                const UnexpectedToken& detail = GetUnexpectedToken();

                return fmt::format("Expected token of type {:s} but got {:s}",
                                   magic_enum::enum_name(detail.expected_type),
                                   magic_enum::enum_name(detail.actual_type));
            }

            case Type::ReserverdIdentifier: {
                const ReserverdIdentifier& detail = GetReserverIdentifier();

                return fmt::format("'{:s}' is a reserved identifier", detail.identifier);
            }

            case Type::InvalidLabelIdentifier: {
                const InvalidLabelIdentifier& detail = GetInvalidLabelIdentifier();

                return fmt::format("'{:s}' is not a valid label identifier", detail.identifer);
            }

            case Type::LabelAlreadyDefined: {
                const LabelAlreadyDefined& detail = GetLabelAlreadyDefined();

                return fmt::format("Label '{:s}' was already defined at {:d}:{:d}",
                                   detail.label_name, detail.at_line, detail.at_column);
            }

            case Type::OneInstructionPerLine: {
                return fmt::format("You may only place one instruction per line");
            }

            case Type::TooFewArgument: {
                const TooFewArguments& detail = GetTooFewArguments();

                return fmt::format(
                        "Not enough arguments provided. Required {:d}, provided only {:d}",
                        detail.required, detail.provided);
            }

            case Type::EmptyLabel: {
                const EmptyLabel& detail = GetEmptyLabel();

                return fmt::format("Label '{:s}' does not have any instruction", detail.label_name);
            }

#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_ASSERT_NOT_REACHED();
#endif
        }
    }

    const ParseError::UnexpectedArgumentType& ParseError::GetUnexpectedArgumentType() const noexcept
    {
        PHI_ASSERT(m_Type == Type::UnexpectedArgumentType);

        return unexpected_argument_type;
    }

    const ParseError::InvalidNumber& ParseError::GetInvalidNumber() const noexcept
    {
        PHI_ASSERT(m_Type == Type::InvalidNumber);

        return invalid_number;
    }

    const ParseError::UnexpectedToken& ParseError::GetUnexpectedToken() const noexcept
    {
        PHI_ASSERT(m_Type == Type::UnexpectedToken);

        return unexpected_token;
    }

    const ParseError::ReserverdIdentifier& ParseError::ParseError::GetReserverIdentifier()
            const noexcept
    {
        PHI_ASSERT(m_Type == Type::ReserverdIdentifier);

        return reserverd_identifier;
    }

    const ParseError::InvalidLabelIdentifier& ParseError::GetInvalidLabelIdentifier() const noexcept
    {
        PHI_ASSERT(m_Type == Type::InvalidLabelIdentifier);

        return invalid_label_identifier;
    }

    const ParseError::LabelAlreadyDefined& ParseError::GetLabelAlreadyDefined() const noexcept
    {
        PHI_ASSERT(m_Type == Type::LabelAlreadyDefined);

        return label_already_defined;
    }

    const ParseError::TooFewArguments& ParseError::GetTooFewArguments() const noexcept
    {
        PHI_ASSERT(m_Type == Type::TooFewArgument);

        return too_few_arguments;
    }

    const ParseError::EmptyLabel& ParseError::GetEmptyLabel() const noexcept
    {
        PHI_ASSERT(m_Type == Type::EmptyLabel);

        return empty_label;
    }

    // Constructor functions

    ParseError ConstructUnexpectedArgumentTypeParseError(std::uint32_t line_number,
                                                         std::uint32_t column,
                                                         ArgumentType  expected_type,
                                                         ArgumentType  actual_type) noexcept
    {
        ParseError err;

        err.m_Type                                 = ParseError::Type::UnexpectedArgumentType;
        err.m_LineNumber                           = line_number;
        err.m_Column                               = column;
        err.unexpected_argument_type.expected_type = expected_type;
        err.unexpected_argument_type.actual_type   = actual_type;

        return err;
    }

    ParseError ConstructUnexpectedArgumentTypeParseError(const Token& token,
                                                         ArgumentType expected_type,
                                                         ArgumentType actual_type) noexcept
    {
        return ConstructUnexpectedArgumentTypeParseError(
                token.GetLineNumber().get(), token.GetColumn().get(), expected_type, actual_type);
    }

    ParseError ConstructInvalidNumberParseError(std::uint32_t line_number, std::uint32_t column,
                                                std::string_view text) noexcept
    {
        ParseError err;

        err.m_Type              = ParseError::Type::InvalidNumber;
        err.m_LineNumber        = line_number;
        err.m_Column            = column;
        err.invalid_number.text = text;

        return err;
    }

    ParseError ConstructInvalidNumberParseError(const Token& token) noexcept
    {
        return ConstructInvalidNumberParseError(token.GetLineNumber().get(),
                                                token.GetColumn().get(), token.GetText());
    }

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(std::uint32_t line_number,
                                                                     std::uint32_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::TooFewArgumentsAddressDisplacement;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(const Token& token) noexcept
    {
        return ConstructTooFewArgumentsAddressDisplacementParseError(token.GetLineNumber().get(),
                                                                     token.GetColumn().get());
    }

    ParseError ConstructUnexpectedTokenParseError(std::uint32_t line_number, std::uint32_t column,
                                                  Token::Type expected_type,
                                                  Token::Type actual_type) noexcept
    {
        ParseError err;

        err.m_Type                         = ParseError::Type::UnexpectedToken;
        err.m_LineNumber                   = line_number;
        err.m_Column                       = column;
        err.unexpected_token.expected_type = expected_type;
        err.unexpected_token.actual_type   = actual_type;

        return err;
    }

    ParseError ConstructUnexpectedTokenParseError(const Token& token,
                                                  Token::Type  expected_type) noexcept
    {
        return ConstructUnexpectedTokenParseError(token.GetLineNumber().get(),
                                                  token.GetColumn().get(), expected_type,
                                                  token.GetType());
    }

    ParseError ConstructReservedIdentiferParseError(std::uint32_t line_number, std::uint32_t column,
                                                    std::string_view identifier) noexcept
    {
        ParseError err;

        err.m_Type                          = ParseError::Type::ReserverdIdentifier;
        err.m_LineNumber                    = line_number;
        err.m_Column                        = column;
        err.reserverd_identifier.identifier = identifier;

        return err;
    }

    ParseError ConstructReservedIdentiferParseError(const Token& token) noexcept
    {
        return ConstructReservedIdentiferParseError(token.GetLineNumber().get(),
                                                    token.GetColumn().get(), token.GetText());
    }

    ParseError ConstructInvalidLabelIdentifierParseError(std::uint32_t    line_number,
                                                         std::uint32_t    column,
                                                         std::string_view identifier) noexcept
    {
        ParseError err;

        err.m_Type                             = ParseError::Type::InvalidLabelIdentifier;
        err.m_LineNumber                       = line_number;
        err.m_Column                           = column;
        err.invalid_label_identifier.identifer = identifier;

        return err;
    }

    ParseError ConstructInvalidLabelIdentifierParseError(const Token& token) noexcept
    {
        return ConstructInvalidLabelIdentifierParseError(token.GetLineNumber().get(),
                                                         token.GetColumn().get(), token.GetText());
    }

    ParseError ConstructLabelAlreadyDefinedParseError(std::uint32_t    line_number,
                                                      std::uint32_t    column,
                                                      std::string_view label_name,
                                                      std::uint32_t    at_line,
                                                      std::uint32_t    at_column) noexcept
    {
        ParseError err;

        err.m_Type                           = ParseError::Type::LabelAlreadyDefined;
        err.m_LineNumber                     = line_number;
        err.m_Column                         = column;
        err.label_already_defined.label_name = label_name;
        err.label_already_defined.at_line    = at_line;
        err.label_already_defined.at_column  = at_column;

        return err;
    }

    ParseError ConstructLabelAlreadyDefinedParseError(const Token& token,
                                                      const Token& first_definition) noexcept
    {
        return ConstructLabelAlreadyDefinedParseError(
                token.GetLineNumber().get(), token.GetColumn().get(), token.GetText(),
                first_definition.GetLineNumber().get(), first_definition.GetColumn().get());
    }

    ParseError ConstructOneInstructionPerLineParseError(std::uint32_t line_number,
                                                        std::uint32_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::OneInstructionPerLine;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    ParseError ConstructOneInstructionPerLineParseError(const Token& token) noexcept
    {
        return ConstructOneInstructionPerLineParseError(token.GetLineNumber().get(),
                                                        token.GetColumn().get());
    }

    ParseError ConstructTooFewArgumentsParseError(std::uint32_t line_number, std::uint32_t column,
                                                  std::uint8_t required,
                                                  std::uint8_t provided) noexcept
    {
        ParseError err;

        err.m_Type                     = ParseError::Type::TooFewArgument;
        err.m_LineNumber               = line_number;
        err.m_Column                   = column;
        err.too_few_arguments.required = required;
        err.too_few_arguments.provided = provided;

        return err;
    }

    ParseError ConstructTooFewArgumentsParseError(const Token& token, std::uint8_t required,
                                                  std::uint8_t provided) noexcept
    {
        return ConstructTooFewArgumentsParseError(token.GetLineNumber().get(),
                                                  token.GetColumn().get(), required, provided);
    }

    ParseError ConstructEmptyLabelParseError(std::uint32_t line_number, std::uint32_t column,
                                             std::string_view label_name) noexcept
    {
        ParseError err;

        err.m_Type                 = ParseError::Type::EmptyLabel;
        err.m_LineNumber           = line_number;
        err.m_Column               = column;
        err.empty_label.label_name = label_name;

        return err;
    }

    ParseError ConstructEmptyLabelParseError(const Token& token) noexcept
    {
        return ConstructEmptyLabelParseError(
                token.GetLineNumber().get(), token.GetColumn().get(),
                (token.GetText().back() == ':') ?
                        token.GetText().substr(0, token.GetText().size() - 1) :
                        token.GetText());
    }

    ParseError ConstructTooManyCommaParseError(std::uint32_t line_number,
                                               std::uint32_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::TooManyComma;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    ParseError ConstructTooManyCommaParseError(const Token& token) noexcept
    {
        return ConstructTooManyCommaParseError(token.GetLineNumber().get(),
                                               token.GetColumn().get());
    }
} // namespace dlx
