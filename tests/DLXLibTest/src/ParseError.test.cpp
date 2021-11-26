#include <catch2/catch_test_macros.hpp>

#include <DLX/InstructionInfo.hpp>
#include <DLX/ParseError.hpp>
#include <DLX/Token.hpp>

TEST_CASE("ParseError")
{
    SECTION("UnexpectedArgumentType")
    {
        {
            dlx::ParseError err = dlx::ConstructUnexpectedArgumentTypeParseError(
                    1, 2, dlx::ArgumentType::FloatRegister, dlx::ArgumentType::Label);

            CHECK(err.GetType() == dlx::ParseError::Type::UnexpectedArgumentType);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::UnexpectedArgumentType& detail = err.GetUnexpectedArgumentType();
            CHECK(detail.expected_type == dlx::ArgumentType::FloatRegister);
            CHECK(detail.actual_type == dlx::ArgumentType::Label);
        }

        {
            dlx::Token      token(dlx::Token::Type::LabelIdentifier, "l", 1u, 2u);
            dlx::ParseError err = dlx::ConstructUnexpectedArgumentTypeParseError(
                    token, dlx::ArgumentType::IntRegister, dlx::ArgumentType::Label);

            CHECK(err.GetType() == dlx::ParseError::Type::UnexpectedArgumentType);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::UnexpectedArgumentType& detail = err.GetUnexpectedArgumentType();
            CHECK(detail.expected_type == dlx::ArgumentType::IntRegister);
            CHECK(detail.actual_type == dlx::ArgumentType::Label);
        }
    }

    SECTION("InvalidNumber")
    {
        {
            dlx::ParseError err = dlx::ConstructInvalidNumberParseError(1, 2, "0b");
            CHECK(err.GetType() == dlx::ParseError::Type::InvalidNumber);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::InvalidNumber& detail = err.GetInvalidNumber();
            CHECK(detail.text == "0b");
        }

        {
            dlx::Token      token{dlx::Token::Type::Colon, "0b", 1u, 2u};
            dlx::ParseError err = dlx::ConstructInvalidNumberParseError(token);

            CHECK(err.GetType() == dlx::ParseError::Type::InvalidNumber);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::InvalidNumber& detail = err.GetInvalidNumber();
            CHECK(detail.text == "0b");
        }
    }

    SECTION("TooFewArgumentsAddressDisplacment")
    {
        {
            dlx::ParseError err = dlx::ConstructTooFewArgumentsAddressDisplacementParseError(1, 2);

            CHECK(err.GetType() == dlx::ParseError::Type::TooFewArgumentsAddressDisplacement);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);
        }

        {
            dlx::Token      token{dlx::Token::Type::ImmediateInteger, "", 1u, 2u};
            dlx::ParseError err = dlx::ConstructTooFewArgumentsAddressDisplacementParseError(token);

            CHECK(err.GetType() == dlx::ParseError::Type::TooFewArgumentsAddressDisplacement);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);
        }
    }

    SECTION("UnexpectedToken")
    {
        {
            dlx::ParseError err = dlx::ConstructUnexpectedTokenParseError(
                    1, 2, dlx::Token::Type::NewLine, dlx::Token::Type::OpCode);

            CHECK(err.GetType() == dlx::ParseError::Type::UnexpectedToken);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::UnexpectedToken& detail = err.GetUnexpectedToken();
            CHECK(detail.expected_type == dlx::Token::Type::NewLine);
            CHECK(detail.actual_type == dlx::Token::Type::OpCode);
        }

        {
            dlx::Token      token{dlx::Token::Type::OpCode, "ADD", 1u, 2u};
            dlx::ParseError err =
                    dlx::ConstructUnexpectedTokenParseError(token, dlx::Token::Type::NewLine);

            CHECK(err.GetType() == dlx::ParseError::Type::UnexpectedToken);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::UnexpectedToken& detail = err.GetUnexpectedToken();
            CHECK(detail.expected_type == dlx::Token::Type::NewLine);
            CHECK(detail.actual_type == dlx::Token::Type::OpCode);
        }
    }

    SECTION("ReservedIdentifier")
    {
        {
            dlx::ParseError err = dlx::ConstructReservedIdentiferParseError(1, 2, "J");

            CHECK(err.GetType() == dlx::ParseError::Type::ReserverdIdentifier);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::ReserverdIdentifier& detail = err.GetReserverIdentifier();
            CHECK(detail.identifier == "J");
        }

        {
            dlx::Token      token{dlx::Token::Type::OpCode, "J", 1u, 2u};
            dlx::ParseError err = dlx::ConstructReservedIdentiferParseError(token);

            CHECK(err.GetType() == dlx::ParseError::Type::ReserverdIdentifier);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::ReserverdIdentifier& detail = err.GetReserverIdentifier();
            CHECK(detail.identifier == "J");
        }
    }

    SECTION("InvalidLabelIdentifier")
    {
        {
            dlx::ParseError err = dlx::ConstructInvalidLabelIdentifierParseError(1, 2, "123_");

            CHECK(err.GetType() == dlx::ParseError::Type::InvalidLabelIdentifier);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::InvalidLabelIdentifier& detail = err.GetInvalidLabelIdentifier();
            CHECK(detail.identifer == "123_");
        }

        {
            dlx::Token      token{dlx::Token::Type::LabelIdentifier, "123_", 1u, 2u};
            dlx::ParseError err = dlx::ConstructInvalidLabelIdentifierParseError(token);

            CHECK(err.GetType() == dlx::ParseError::Type::InvalidLabelIdentifier);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::InvalidLabelIdentifier& detail = err.GetInvalidLabelIdentifier();
            CHECK(detail.identifer == "123_");
        }
    }

    SECTION("LabelAlreadyDefined")
    {
        {
            dlx::ParseError err = dlx::ConstructLabelAlreadyDefinedParseError(1, 2, "l", 3, 4);

            CHECK(err.GetType() == dlx::ParseError::Type::LabelAlreadyDefined);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::LabelAlreadyDefined& detail = err.GetLabelAlreadyDefined();
            CHECK(detail.label_name == "l");
            CHECK(detail.at_line == 3);
            CHECK(detail.at_column == 4);
        }

        {
            dlx::Token first{dlx::Token::Type::LabelIdentifier, "l", 1u, 2u};
            dlx::Token second{dlx::Token::Type::LabelIdentifier, "l", 3u, 4u};

            dlx::ParseError err = dlx::ConstructLabelAlreadyDefinedParseError(first, second);

            CHECK(err.GetType() == dlx::ParseError::Type::LabelAlreadyDefined);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::LabelAlreadyDefined& detail = err.GetLabelAlreadyDefined();
            CHECK(detail.label_name == "l");
            CHECK(detail.at_line == 3);
            CHECK(detail.at_column == 4);
        }
    }

    SECTION("OneInstructionPerLine")
    {
        {
            dlx::ParseError err = dlx::ConstructOneInstructionPerLineParseError(1, 2);

            CHECK(err.GetType() == dlx::ParseError::Type::OneInstructionPerLine);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);
        }

        {
            dlx::Token      token{dlx::Token::Type::LabelIdentifier, "l", 1u, 2u};
            dlx::ParseError err = dlx::ConstructOneInstructionPerLineParseError(token);

            CHECK(err.GetType() == dlx::ParseError::Type::OneInstructionPerLine);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);
        }
    }

    SECTION("TooFewArguments")
    {
        {
            dlx::ParseError err = dlx::ConstructTooFewArgumentsParseError(1, 2, 3, 2);

            CHECK(err.GetType() == dlx::ParseError::Type::TooFewArgument);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::TooFewArguments& detail = err.GetTooFewArguments();
            CHECK(detail.required == 3);
            CHECK(detail.provided == 2);
        }

        {
            dlx::Token      token{dlx::Token::Type::Comma, ",", 1u, 2u};
            dlx::ParseError err = dlx::ConstructTooFewArgumentsParseError(token, 3, 2);

            CHECK(err.GetType() == dlx::ParseError::Type::TooFewArgument);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::TooFewArguments& detail = err.GetTooFewArguments();
            CHECK(detail.required == 3);
            CHECK(detail.provided == 2);
        }
    }

    SECTION("EmptyLabel")
    {
        {
            dlx::ParseError err = dlx::ConstructEmptyLabelParseError(1, 2, "l");

            CHECK(err.GetType() == dlx::ParseError::Type::EmptyLabel);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::EmptyLabel& detail = err.GetEmptyLabel();
            CHECK(detail.label_name == "l");
        }

        {
            dlx::Token      token{dlx::Token::Type::Comma, "l", 1u, 2u};
            dlx::ParseError err = dlx::ConstructEmptyLabelParseError(token);

            CHECK(err.GetType() == dlx::ParseError::Type::EmptyLabel);
            CHECK(err.GetLineNumber() == 1);
            CHECK(err.GetColumn() == 2);

            const dlx::ParseError::EmptyLabel& detail = err.GetEmptyLabel();
            CHECK(detail.label_name == "l");
        }
    }
}
