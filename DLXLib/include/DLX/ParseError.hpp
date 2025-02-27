#pragma once

#include "DLX/InstructionInfo.hpp"
#include "DLX/Token.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/sized_types.hpp>
#include <string>

namespace dlx
{
    class ParseError
    {
    public:
        enum class Type
        {
            UnexpectedArgumentType,
            InvalidNumber,
            TooFewArgumentsAddressDisplacement,
            UnexpectedToken,
            ReservedIdentifier,
            InvalidLabelIdentifier,
            LabelAlreadyDefined,
            OneInstructionPerLine,
            TooFewArgument,
            EmptyLabel,
            TooManyComma,

            MAX_ITEMS,
        };

        struct UnexpectedArgumentType
        {
            ArgumentType expected_type;
            ArgumentType actual_type;
        };

        struct InvalidNumber
        {
            phi::string_view text;
        };

        struct UnexpectedToken
        {
            Token::Type expected_type;
            Token::Type actual_type;
        };

        struct ReservedIdentifier
        {
            phi::string_view identifier;
        };

        struct InvalidLabelIdentifier
        {
            phi::string_view identifier;
        };

        struct LabelAlreadyDefined
        {
            phi::string_view label_name;
            phi::uint64_t    at_line;
            phi::uint64_t    at_column;
        };

        struct TooFewArguments
        {
            phi::uint8_t required;
            phi::uint8_t provided;
        };

        struct EmptyLabel
        {
            phi::string_view label_name;
        };

    public:
        [[nodiscard]] Type GetType() const noexcept;

        [[nodiscard]] phi::uint64_t GetLineNumber() const noexcept;

        [[nodiscard]] phi::uint64_t GetColumn() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

        [[nodiscard]] std::string ConstructMessage() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_POP()

        [[nodiscard]] const UnexpectedArgumentType& GetUnexpectedArgumentType() const noexcept;

        [[nodiscard]] const InvalidNumber& GetInvalidNumber() const noexcept;

        [[nodiscard]] const UnexpectedToken& GetUnexpectedToken() const noexcept;

        [[nodiscard]] const ReservedIdentifier& GetReserverIdentifier() const noexcept;

        [[nodiscard]] const InvalidLabelIdentifier& GetInvalidLabelIdentifier() const noexcept;

        [[nodiscard]] const LabelAlreadyDefined& GetLabelAlreadyDefined() const noexcept;

        [[nodiscard]] const TooFewArguments& GetTooFewArguments() const noexcept;

        [[nodiscard]] const EmptyLabel& GetEmptyLabel() const noexcept;

    private:
        ParseError() noexcept;

        Type          m_Type;
        phi::uint64_t m_LineNumber;
        phi::uint64_t m_Column;

        PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4582) // 'x': constructor is not implicitly called

        union
        {
            UnexpectedArgumentType unexpected_argument_type;
            InvalidNumber          invalid_number;
            UnexpectedToken        unexpected_token;
            ReservedIdentifier     reserved_identifier;
            InvalidLabelIdentifier invalid_label_identifier;
            LabelAlreadyDefined    label_already_defined;
            TooFewArguments        too_few_arguments;
            EmptyLabel             empty_label;
        };

        PHI_MSVC_SUPPRESS_WARNING_POP()

        friend ParseError ConstructUnexpectedArgumentTypeParseError(
                phi::uint64_t line_number, phi::uint64_t column, ArgumentType expected_type,
                ArgumentType actual_type) noexcept;

        friend ParseError ConstructInvalidNumberParseError(phi::uint64_t    line_number,
                                                           phi::uint64_t    column,
                                                           phi::string_view text) noexcept;

        friend ParseError ConstructTooFewArgumentsAddressDisplacementParseError(
                phi::uint64_t line_number, phi::uint64_t column) noexcept;

        friend ParseError ConstructUnexpectedTokenParseError(phi::uint64_t line_number,
                                                             phi::uint64_t column,
                                                             Token::Type   expected_type,
                                                             Token::Type   actual_type) noexcept;

        friend ParseError ConstructReservedIdentifierParseError(
                phi::uint64_t line_number, phi::uint64_t column,
                phi::string_view identifier) noexcept;

        friend ParseError ConstructInvalidLabelIdentifierParseError(
                phi::uint64_t line_number, phi::uint64_t column,
                phi::string_view identifier) noexcept;

        friend ParseError ConstructLabelAlreadyDefinedParseError(phi::uint64_t    line_number,
                                                                 phi::uint64_t    column,
                                                                 phi::string_view label_name,
                                                                 phi::uint64_t    at_line,
                                                                 phi::uint64_t at_column) noexcept;

        friend ParseError ConstructOneInstructionPerLineParseError(phi::uint64_t line_number,
                                                                   phi::uint64_t column) noexcept;

        friend ParseError ConstructTooFewArgumentsParseError(phi::uint64_t line_number,
                                                             phi::uint64_t column,
                                                             phi::uint8_t  required,
                                                             phi::uint8_t  provided) noexcept;

        friend ParseError ConstructEmptyLabelParseError(phi::uint64_t    line_number,
                                                        phi::uint64_t    column,
                                                        phi::string_view label_name) noexcept;

        friend ParseError ConstructTooManyCommaParseError(phi::uint64_t line_number,
                                                          phi::uint64_t column) noexcept;
    };

    ParseError ConstructUnexpectedArgumentTypeParseError(phi::uint64_t line_number,
                                                         phi::uint64_t column,
                                                         ArgumentType  expected_type,
                                                         ArgumentType  actual_type) noexcept;

    ParseError ConstructUnexpectedArgumentTypeParseError(const Token& token,
                                                         ArgumentType expected_type,
                                                         ArgumentType actual_type) noexcept;

    ParseError ConstructInvalidNumberParseError(phi::uint64_t line_number, phi::uint64_t column,
                                                phi::string_view text) noexcept;

    ParseError ConstructInvalidNumberParseError(const Token& token) noexcept;

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(phi::uint64_t line_number,
                                                                     phi::uint64_t column) noexcept;

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(const Token& token) noexcept;

    ParseError ConstructUnexpectedTokenParseError(phi::uint64_t line_number, phi::uint64_t column,
                                                  Token::Type expected_type,
                                                  Token::Type actual_type) noexcept;

    ParseError ConstructUnexpectedTokenParseError(const Token& token,
                                                  Token::Type  expected_type) noexcept;

    ParseError ConstructReservedIdentifierParseError(phi::uint64_t    line_number,
                                                     phi::uint64_t    column,
                                                     phi::string_view identifier) noexcept;

    ParseError ConstructReservedIdentifierParseError(const Token& token) noexcept;

    ParseError ConstructInvalidLabelIdentifierParseError(phi::uint64_t    line_number,
                                                         phi::uint64_t    column,
                                                         phi::string_view identifier) noexcept;

    ParseError ConstructInvalidLabelIdentifierParseError(const Token& token) noexcept;

    ParseError ConstructLabelAlreadyDefinedParseError(phi::uint64_t    line_number,
                                                      phi::uint64_t    column,
                                                      phi::string_view label_name,
                                                      phi::uint64_t    at_line,
                                                      phi::uint64_t    at_column) noexcept;

    ParseError ConstructLabelAlreadyDefinedParseError(const Token& token,
                                                      const Token& first_definition) noexcept;

    ParseError ConstructOneInstructionPerLineParseError(phi::uint64_t line_number,
                                                        phi::uint64_t column) noexcept;

    ParseError ConstructOneInstructionPerLineParseError(const Token& token) noexcept;

    ParseError ConstructTooFewArgumentsParseError(phi::uint64_t line_number, phi::uint64_t column,
                                                  phi::uint8_t required,
                                                  phi::uint8_t provided) noexcept;

    ParseError ConstructTooFewArgumentsParseError(const Token& token, phi::uint8_t required,
                                                  phi::uint8_t provided) noexcept;

    ParseError ConstructEmptyLabelParseError(phi::uint64_t line_number, phi::uint64_t column,
                                             phi::string_view label_name) noexcept;

    ParseError ConstructEmptyLabelParseError(const Token& token) noexcept;

    ParseError ConstructTooManyCommaParseError(phi::uint64_t line_number,
                                               phi::uint64_t column) noexcept;

    ParseError ConstructTooManyCommaParseError(const Token& token) noexcept;
} // namespace dlx
