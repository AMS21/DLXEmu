#include "DLX/ParseError.hpp"

#include "DLX/InstructionInfo.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

PHI_MSVC_SUPPRESS_WARNING(4582)

namespace dlx
{
    ParseError::ParseError() noexcept
    {}

    PHI_ATTRIBUTE_CONST ParseError::Type ParseError::GetType() const noexcept
    {
        return m_Type;
    }

    PHI_ATTRIBUTE_CONST phi::uint64_t ParseError::GetLineNumber() const noexcept
    {
        return m_LineNumber;
    }

    PHI_ATTRIBUTE_CONST phi::uint64_t ParseError::GetColumn() const noexcept
    {
        return m_Column;
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wswitch")
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wreturn-type")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

    std::string ParseError::ConstructMessage() const noexcept
    {
        switch (m_Type)
        {
            case Type::UnexpectedArgumentType: {
                const UnexpectedArgumentType& detail = GetUnexpectedArgumentType();

                return fmt::format("Expected {:s} but got {:s}",
                                   dlx::enum_name(detail.expected_type),
                                   dlx::enum_name(detail.actual_type));
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
                                   dlx::enum_name(detail.expected_type),
                                   dlx::enum_name(detail.actual_type));
            }

            case Type::ReserverdIdentifier: {
                const ReservedIdentifier& detail = GetReserverIdentifier();

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

            case Type::TooManyComma: {
                return fmt::format("Only one comma is allowed");
            }

#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_ASSERT_NOT_REACHED();
#endif
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

    PHI_ATTRIBUTE_CONST const ParseError::UnexpectedArgumentType& ParseError::
            GetUnexpectedArgumentType() const noexcept
    {
        PHI_ASSERT(m_Type == Type::UnexpectedArgumentType);

        return unexpected_argument_type;
    }

    PHI_ATTRIBUTE_CONST const ParseError::InvalidNumber& ParseError::GetInvalidNumber()
            const noexcept
    {
        PHI_ASSERT(m_Type == Type::InvalidNumber);

        return invalid_number;
    }

    PHI_ATTRIBUTE_CONST const ParseError::UnexpectedToken& ParseError::GetUnexpectedToken()
            const noexcept
    {
        PHI_ASSERT(m_Type == Type::UnexpectedToken);

        return unexpected_token;
    }

    PHI_ATTRIBUTE_CONST const ParseError::ReservedIdentifier& ParseError::ParseError::
            GetReserverIdentifier() const noexcept
    {
        PHI_ASSERT(m_Type == Type::ReserverdIdentifier);

        return reserved_identifier;
    }

    PHI_ATTRIBUTE_CONST const ParseError::InvalidLabelIdentifier& ParseError::
            GetInvalidLabelIdentifier() const noexcept
    {
        PHI_ASSERT(m_Type == Type::InvalidLabelIdentifier);

        return invalid_label_identifier;
    }

    PHI_ATTRIBUTE_CONST const ParseError::LabelAlreadyDefined& ParseError::GetLabelAlreadyDefined()
            const noexcept
    {
        PHI_ASSERT(m_Type == Type::LabelAlreadyDefined);

        return label_already_defined;
    }

    PHI_ATTRIBUTE_CONST const ParseError::TooFewArguments& ParseError::GetTooFewArguments()
            const noexcept
    {
        PHI_ASSERT(m_Type == Type::TooFewArgument);

        return too_few_arguments;
    }

    PHI_ATTRIBUTE_CONST const ParseError::EmptyLabel& ParseError::GetEmptyLabel() const noexcept
    {
        PHI_ASSERT(m_Type == Type::EmptyLabel);

        return empty_label;
    }

    // Constructor functions

    PHI_ATTRIBUTE_CONST ParseError ConstructUnexpectedArgumentTypeParseError(
            phi::uint64_t line_number, phi::uint64_t column, ArgumentType expected_type,
            ArgumentType actual_type) noexcept
    {
        ParseError err;

        err.m_Type                                 = ParseError::Type::UnexpectedArgumentType;
        err.m_LineNumber                           = line_number;
        err.m_Column                               = column;
        err.unexpected_argument_type.expected_type = expected_type;
        err.unexpected_argument_type.actual_type   = actual_type;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructUnexpectedArgumentTypeParseError(
            const Token& token, ArgumentType expected_type, ArgumentType actual_type) noexcept
    {
        return ConstructUnexpectedArgumentTypeParseError(token.GetLineNumber().unsafe(),
                                                         token.GetColumn().unsafe(), expected_type,
                                                         actual_type);
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructInvalidNumberParseError(phi::uint64_t    line_number,
                                                                    phi::uint64_t    column,
                                                                    std::string_view text) noexcept
    {
        ParseError err;

        err.m_Type              = ParseError::Type::InvalidNumber;
        err.m_LineNumber        = line_number;
        err.m_Column            = column;
        err.invalid_number.text = text;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructInvalidNumberParseError(const Token& token) noexcept
    {
        return ConstructInvalidNumberParseError(token.GetLineNumber().unsafe(),
                                                token.GetColumn().unsafe(), token.GetText());
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructTooFewArgumentsAddressDisplacementParseError(
            phi::uint64_t line_number, phi::uint64_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::TooFewArgumentsAddressDisplacement;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError
    ConstructTooFewArgumentsAddressDisplacementParseError(const Token& token) noexcept
    {
        return ConstructTooFewArgumentsAddressDisplacementParseError(token.GetLineNumber().unsafe(),
                                                                     token.GetColumn().unsafe());
    }

    PHI_ATTRIBUTE_CONST ParseError
    ConstructUnexpectedTokenParseError(phi::uint64_t line_number, phi::uint64_t column,
                                       Token::Type expected_type, Token::Type actual_type) noexcept
    {
        ParseError err;

        err.m_Type                         = ParseError::Type::UnexpectedToken;
        err.m_LineNumber                   = line_number;
        err.m_Column                       = column;
        err.unexpected_token.expected_type = expected_type;
        err.unexpected_token.actual_type   = actual_type;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError
    ConstructUnexpectedTokenParseError(const Token& token, Token::Type expected_type) noexcept
    {
        return ConstructUnexpectedTokenParseError(token.GetLineNumber().unsafe(),
                                                  token.GetColumn().unsafe(), expected_type,
                                                  token.GetType());
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructReservedIdentiferParseError(
            phi::uint64_t line_number, phi::uint64_t column, std::string_view identifier) noexcept
    {
        ParseError err;

        err.m_Type                         = ParseError::Type::ReserverdIdentifier;
        err.m_LineNumber                   = line_number;
        err.m_Column                       = column;
        err.reserved_identifier.identifier = identifier;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructReservedIdentiferParseError(const Token& token) noexcept
    {
        return ConstructReservedIdentiferParseError(token.GetLineNumber().unsafe(),
                                                    token.GetColumn().unsafe(), token.GetText());
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructInvalidLabelIdentifierParseError(
            phi::uint64_t line_number, phi::uint64_t column, std::string_view identifier) noexcept
    {
        ParseError err;

        err.m_Type                             = ParseError::Type::InvalidLabelIdentifier;
        err.m_LineNumber                       = line_number;
        err.m_Column                           = column;
        err.invalid_label_identifier.identifer = identifier;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError
    ConstructInvalidLabelIdentifierParseError(const Token& token) noexcept
    {
        return ConstructInvalidLabelIdentifierParseError(
                token.GetLineNumber().unsafe(), token.GetColumn().unsafe(), token.GetText());
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructLabelAlreadyDefinedParseError(
            phi::uint64_t line_number, phi::uint64_t column, std::string_view label_name,
            phi::uint64_t at_line, phi::uint64_t at_column) noexcept
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

    PHI_ATTRIBUTE_CONST ParseError ConstructLabelAlreadyDefinedParseError(
            const Token& token, const Token& first_definition) noexcept
    {
        return ConstructLabelAlreadyDefinedParseError(
                token.GetLineNumber().unsafe(), token.GetColumn().unsafe(), token.GetText(),
                first_definition.GetLineNumber().unsafe(), first_definition.GetColumn().unsafe());
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructOneInstructionPerLineParseError(
            phi::uint64_t line_number, phi::uint64_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::OneInstructionPerLine;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError
    ConstructOneInstructionPerLineParseError(const Token& token) noexcept
    {
        return ConstructOneInstructionPerLineParseError(token.GetLineNumber().unsafe(),
                                                        token.GetColumn().unsafe());
    }

    PHI_ATTRIBUTE_CONST ParseError
    ConstructTooFewArgumentsParseError(phi::uint64_t line_number, phi::uint64_t column,
                                       phi::uint8_t required, phi::uint8_t provided) noexcept
    {
        ParseError err;

        err.m_Type                     = ParseError::Type::TooFewArgument;
        err.m_LineNumber               = line_number;
        err.m_Column                   = column;
        err.too_few_arguments.required = required;
        err.too_few_arguments.provided = provided;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructTooFewArgumentsParseError(
            const Token& token, std::uint8_t required, std::uint8_t provided) noexcept
    {
        return ConstructTooFewArgumentsParseError(token.GetLineNumber().unsafe(),
                                                  token.GetColumn().unsafe(), required, provided);
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructEmptyLabelParseError(
            phi::uint64_t line_number, phi::uint64_t column, std::string_view label_name) noexcept
    {
        ParseError err;

        err.m_Type                 = ParseError::Type::EmptyLabel;
        err.m_LineNumber           = line_number;
        err.m_Column               = column;
        err.empty_label.label_name = label_name;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructEmptyLabelParseError(const Token& token) noexcept
    {
        return ConstructEmptyLabelParseError(
                token.GetLineNumber().unsafe(), token.GetColumn().unsafe(),
                (token.GetText().back() == ':') ?
                        token.GetText().substr(0, token.GetText().size() - 1) :
                        token.GetText());
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructTooManyCommaParseError(phi::uint64_t line_number,
                                                                   phi::uint64_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::TooManyComma;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    PHI_ATTRIBUTE_CONST ParseError ConstructTooManyCommaParseError(const Token& token) noexcept
    {
        return ConstructTooManyCommaParseError(token.GetLineNumber().unsafe(),
                                               token.GetColumn().unsafe());
    }
} // namespace dlx
