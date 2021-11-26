#pragma once

#include "DLX/InstructionInfo.hpp"
#include "DLX/Token.hpp"
#include <cstdint>
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
            ReserverdIdentifier,
            InvalidLabelIdentifier,
            LabelAlreadyDefined,
            OneInstructionPerLine,
            TooFewArgument,
            EmptyLabel,

            MAX_ITEMS,
        };

        struct UnexpectedArgumentType
        {
            ArgumentType expected_type;
            ArgumentType actual_type;
        };

        struct InvalidNumber
        {
            std::string_view text;
        };

        struct UnexpectedToken
        {
            Token::Type expected_type;
            Token::Type actual_type;
        };

        struct ReserverdIdentifier
        {
            std::string_view identifier;
        };

        struct InvalidLabelIdentifier
        {
            std::string_view identifer;
        };

        struct LabelAlreadyDefined
        {
            std::string_view label_name;
            std::uint32_t    at_line;
            std::uint32_t    at_column;
        };

        struct TooFewArguments
        {
            std::uint8_t required;
            std::uint8_t provided;
        };

        struct EmptyLabel
        {
            std::string_view label_name;
        };

    public:
        [[nodiscard]] Type GetType() const noexcept;

        [[nodiscard]] std::uint32_t GetLineNumber() const noexcept;

        [[nodiscard]] std::uint32_t GetColumn() const noexcept;

        [[nodiscard]] std::string ConstructMessage() const noexcept;

        [[nodiscard]] const UnexpectedArgumentType& GetUnexpectedArgumentType() const noexcept;

        [[nodiscard]] const InvalidNumber& GetInvalidNumber() const noexcept;

        [[nodiscard]] const UnexpectedToken& GetUnexpectedToken() const noexcept;

        [[nodiscard]] const ReserverdIdentifier& GetReserverIdentifier() const noexcept;

        [[nodiscard]] const InvalidLabelIdentifier& GetInvalidLabelIdentifier() const noexcept;

        [[nodiscard]] const LabelAlreadyDefined& GetLabelAlreadyDefined() const noexcept;

        [[nodiscard]] const TooFewArguments& GetTooFewArguments() const noexcept;

        [[nodiscard]] const EmptyLabel& GetEmptyLabel() const noexcept;

    private:
        ParseError() noexcept;

        Type          m_Type;
        std::uint32_t m_LineNumber;
        std::uint32_t m_Column;

        union
        {
            UnexpectedArgumentType unexpected_argument_type;
            InvalidNumber          invalid_number;
            UnexpectedToken        unexpected_token;
            ReserverdIdentifier    reserverd_identifier;
            InvalidLabelIdentifier invalid_label_identifier;
            LabelAlreadyDefined    label_already_defined;
            TooFewArguments        too_few_arguments;
            EmptyLabel             empty_label;
        };

        friend ParseError ConstructUnexpectedArgumentTypeParseError(
                std::uint32_t line_number, std::uint32_t column, ArgumentType expected_type,
                ArgumentType actual_type) noexcept;

        friend ParseError ConstructInvalidNumberParseError(std::uint32_t    line_number,
                                                           std::uint32_t    column,
                                                           std::string_view text) noexcept;

        friend ParseError ConstructTooFewArgumentsAddressDisplacementParseError(
                std::uint32_t line_number, std::uint32_t column) noexcept;

        friend ParseError ConstructUnexpectedTokenParseError(std::uint32_t line_number,
                                                             std::uint32_t column,
                                                             Token::Type   expected_type,
                                                             Token::Type   actual_type) noexcept;

        friend ParseError ConstructReservedIdentiferParseError(
                std::uint32_t line_number, std::uint32_t column,
                std::string_view identifier) noexcept;

        friend ParseError ConstructInvalidLabelIdentifierParseError(
                std::uint32_t line_number, std::uint32_t column,
                std::string_view identifier) noexcept;

        friend ParseError ConstructLabelAlreadyDefinedParseError(std::uint32_t    line_number,
                                                                 std::uint32_t    column,
                                                                 std::string_view label_name,
                                                                 std::uint32_t    at_line,
                                                                 std::uint32_t at_column) noexcept;

        friend ParseError ConstructOneInstructionPerLineParseError(std::uint32_t line_number,
                                                                   std::uint32_t column) noexcept;

        friend ParseError ConstructTooFewArgumentsParseError(std::uint32_t line_number,
                                                             std::uint32_t column,
                                                             std::uint8_t  required,
                                                             std::uint8_t  provided) noexcept;

        friend ParseError ConstructEmptyLabelParseError(std::uint32_t    line_number,
                                                        std::uint32_t    column,
                                                        std::string_view label_name) noexcept;
    };

    ParseError ConstructUnexpectedArgumentTypeParseError(std::uint32_t line_number,
                                                         std::uint32_t column,
                                                         ArgumentType  expected_type,
                                                         ArgumentType  actual_type) noexcept;

    ParseError ConstructUnexpectedArgumentTypeParseError(const Token& token,
                                                         ArgumentType expected_type,
                                                         ArgumentType actual_type) noexcept;

    ParseError ConstructInvalidNumberParseError(std::uint32_t line_number, std::uint32_t column,
                                                std::string_view text) noexcept;

    ParseError ConstructInvalidNumberParseError(const Token& token) noexcept;

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(std::uint32_t line_number,
                                                                     std::uint32_t column) noexcept;

    ParseError ConstructTooFewArgumentsAddressDisplacementParseError(const Token& token) noexcept;

    ParseError ConstructUnexpectedTokenParseError(std::uint32_t line_number, std::uint32_t column,
                                                  Token::Type expected_type,
                                                  Token::Type actual_type) noexcept;

    ParseError ConstructUnexpectedTokenParseError(const Token& token,
                                                  Token::Type  expected_type) noexcept;

    ParseError ConstructReservedIdentiferParseError(std::uint32_t line_number, std::uint32_t column,
                                                    std::string_view identifier) noexcept;

    ParseError ConstructReservedIdentiferParseError(const Token& token) noexcept;

    ParseError ConstructInvalidLabelIdentifierParseError(std::uint32_t    line_number,
                                                         std::uint32_t    column,
                                                         std::string_view identifier) noexcept;

    ParseError ConstructInvalidLabelIdentifierParseError(const Token& token) noexcept;

    ParseError ConstructLabelAlreadyDefinedParseError(std::uint32_t    line_number,
                                                      std::uint32_t    column,
                                                      std::string_view label_name,
                                                      std::uint32_t    at_line,
                                                      std::uint32_t    at_column) noexcept;

    ParseError ConstructLabelAlreadyDefinedParseError(const Token& token,
                                                      const Token& first_definition) noexcept;

    ParseError ConstructOneInstructionPerLineParseError(std::uint32_t line_number,
                                                        std::uint32_t column) noexcept;

    ParseError ConstructOneInstructionPerLineParseError(const Token& token) noexcept;

    ParseError ConstructTooFewArgumentsParseError(std::uint32_t line_number, std::uint32_t column,
                                                  std::uint8_t required,
                                                  std::uint8_t provided) noexcept;

    ParseError ConstructTooFewArgumentsParseError(const Token& token, std::uint8_t required,
                                                  std::uint8_t provided) noexcept;

    ParseError ConstructEmptyLabelParseError(std::uint32_t line_number, std::uint32_t column,
                                             std::string_view label_name) noexcept;

    ParseError ConstructEmptyLabelParseError(const Token& token) noexcept;
} // namespace dlx
