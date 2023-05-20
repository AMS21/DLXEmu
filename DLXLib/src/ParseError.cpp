#include "DLX/ParseError.hpp"

#include "DLX/InstructionInfo.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_MSVC_SUPPRESS_WARNING(4582)

namespace dlx
{
    ParseError::ParseError() noexcept
    {}

    ParseError::Type ParseError::GetType() const noexcept
    {
        return m_Type;
    }

    phi::uint64_t ParseError::GetLineNumber() const noexcept
    {
        return m_LineNumber;
    }

    phi::uint64_t ParseError::GetColumn() const noexcept
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
                                   dlx::enum_name(detail.expected_type).data(),
                                   dlx::enum_name(detail.actual_type).data());
            }

            case Type::InvalidNumber: {
                const InvalidNumber& detail = GetInvalidNumber();

                return fmt::format("'{:s}' is not a valid number", detail.text.data());
            }

            case Type::TooFewArgumentsAddressDisplacement: {
                return fmt::format("Not enough arguments left to parse address displacement");
            }

            case Type::UnexpectedToken: {
                const UnexpectedToken& detail = GetUnexpectedToken();

                return fmt::format("Expected token of type {:s} but got {:s}",
                                   dlx::enum_name(detail.expected_type).data(),
                                   dlx::enum_name(detail.actual_type).data());
            }

            case Type::ReservedIdentifier: {
                const ReservedIdentifier& detail = GetReserverIdentifier();

                return fmt::format("'{:s}' is a reserved identifier", detail.identifier.data());
            }

            case Type::InvalidLabelIdentifier: {
                const InvalidLabelIdentifier& detail = GetInvalidLabelIdentifier();

                return fmt::format("'{:s}' is not a valid label identifier",
                                   detail.identifier.data());
            }

            case Type::LabelAlreadyDefined: {
                const LabelAlreadyDefined& detail = GetLabelAlreadyDefined();

                return fmt::format("Label '{:s}' was already defined at {:d}:{:d}",
                                   detail.label_name.data(), detail.at_line, detail.at_column);
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

                return fmt::format("Label '{:s}' does not have any instruction",
                                   detail.label_name.data());
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

    const ParseError::ReservedIdentifier& ParseError::ParseError::GetReserverIdentifier()
            const noexcept
    {
        PHI_ASSERT(m_Type == Type::ReservedIdentifier);

        return reserved_identifier;
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

    ParseError ConstructUnexpectedArgumentTypeParseError(phi::uint64_t line_number,
                                                         phi::uint64_t column,
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
        return ConstructUnexpectedArgumentTypeParseError(token.GetLineNumber().unsafe(),
                                                         token.GetColumn().unsafe(), expected_type,
                                                         actual_type);
    }

    ParseError ConstructInvalidNumberParseError(phi::uint64_t line_number, phi::uint64_t column,
                                                phi::string_view text) noexcept
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
        return ConstructInvalidNumberParseError(token.GetLineNumber().unsafe(),
                                                token.GetColumn().unsafe(), token.GetText());
    }

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(phi::uint64_t line_number,
                                                                     phi::uint64_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::TooFewArgumentsAddressDisplacement;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(const Token& token) noexcept
    {
        return ConstructTooFewArgumentsAddressDisplacementParseError(token.GetLineNumber().unsafe(),
                                                                     token.GetColumn().unsafe());
    }

    ParseError ConstructUnexpectedTokenParseError(phi::uint64_t line_number, phi::uint64_t column,
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
        return ConstructUnexpectedTokenParseError(token.GetLineNumber().unsafe(),
                                                  token.GetColumn().unsafe(), expected_type,
                                                  token.GetType());
    }

    ParseError ConstructReservedIdentifierParseError(phi::uint64_t    line_number,
                                                     phi::uint64_t    column,
                                                     phi::string_view identifier) noexcept
    {
        ParseError err;

        err.m_Type                         = ParseError::Type::ReservedIdentifier;
        err.m_LineNumber                   = line_number;
        err.m_Column                       = column;
        err.reserved_identifier.identifier = identifier;

        return err;
    }

    ParseError ConstructReservedIdentifierParseError(const Token& token) noexcept
    {
        return ConstructReservedIdentifierParseError(token.GetLineNumber().unsafe(),
                                                     token.GetColumn().unsafe(), token.GetText());
    }

    ParseError ConstructInvalidLabelIdentifierParseError(phi::uint64_t    line_number,
                                                         phi::uint64_t    column,
                                                         phi::string_view identifier) noexcept
    {
        ParseError err;

        err.m_Type                              = ParseError::Type::InvalidLabelIdentifier;
        err.m_LineNumber                        = line_number;
        err.m_Column                            = column;
        err.invalid_label_identifier.identifier = identifier;

        return err;
    }

    ParseError ConstructInvalidLabelIdentifierParseError(const Token& token) noexcept
    {
        return ConstructInvalidLabelIdentifierParseError(
                token.GetLineNumber().unsafe(), token.GetColumn().unsafe(), token.GetText());
    }

    ParseError ConstructLabelAlreadyDefinedParseError(phi::uint64_t    line_number,
                                                      phi::uint64_t    column,
                                                      phi::string_view label_name,
                                                      phi::uint64_t    at_line,
                                                      phi::uint64_t    at_column) noexcept
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
                token.GetLineNumber().unsafe(), token.GetColumn().unsafe(), token.GetText(),
                first_definition.GetLineNumber().unsafe(), first_definition.GetColumn().unsafe());
    }

    ParseError ConstructOneInstructionPerLineParseError(phi::uint64_t line_number,
                                                        phi::uint64_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::OneInstructionPerLine;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    ParseError ConstructOneInstructionPerLineParseError(const Token& token) noexcept
    {
        return ConstructOneInstructionPerLineParseError(token.GetLineNumber().unsafe(),
                                                        token.GetColumn().unsafe());
    }

    ParseError ConstructTooFewArgumentsParseError(phi::uint64_t line_number, phi::uint64_t column,
                                                  phi::uint8_t required,
                                                  phi::uint8_t provided) noexcept
    {
        ParseError err;

        err.m_Type                     = ParseError::Type::TooFewArgument;
        err.m_LineNumber               = line_number;
        err.m_Column                   = column;
        err.too_few_arguments.required = required;
        err.too_few_arguments.provided = provided;

        return err;
    }

    ParseError ConstructTooFewArgumentsParseError(const Token& token, phi::uint8_t required,
                                                  phi::uint8_t provided) noexcept
    {
        return ConstructTooFewArgumentsParseError(token.GetLineNumber().unsafe(),
                                                  token.GetColumn().unsafe(), required, provided);
    }

    ParseError ConstructEmptyLabelParseError(phi::uint64_t line_number, phi::uint64_t column,
                                             phi::string_view label_name) noexcept
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
                token.GetLineNumber().unsafe(), token.GetColumn().unsafe(),
                (token.GetText().back() == ':') ?
                        token.GetText().substring_view(0u, token.GetText().length() - 1u) :
                        token.GetText());
    }

    ParseError ConstructTooManyCommaParseError(phi::uint64_t line_number,
                                               phi::uint64_t column) noexcept
    {
        ParseError err;

        err.m_Type       = ParseError::Type::TooManyComma;
        err.m_LineNumber = line_number;
        err.m_Column     = column;

        return err;
    }

    ParseError ConstructTooManyCommaParseError(const Token& token) noexcept
    {
        return ConstructTooManyCommaParseError(token.GetLineNumber().unsafe(),
                                               token.GetColumn().unsafe());
    }
} // namespace dlx
