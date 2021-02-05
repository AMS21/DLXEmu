#include "DLX/Token.hpp"

#include <magic_enum.hpp>
#include <string>

namespace dlx
{
    Token::Token(Type type, std::string_view text, phi::u64 line_number, phi::u64 column)
        : m_Type{type}
        , m_Text{text}
        , m_LineNumber{line_number}
        , m_Column{column}
    {}

    Token::Type Token::GetType() const noexcept
    {
        return m_Type;
    }

    std::string_view Token::GetTypeName() const noexcept
    {
        return magic_enum::enum_name(m_Type);
    }

    phi::u64 Token::GetLineNumber() const noexcept
    {
        return m_LineNumber;
    }

    phi::u64 Token::GetColumn() const noexcept
    {
        return m_Column;
    }

    phi::usize Token::GetLength() const noexcept
    {
        return m_Text.length();
    }

    std::string_view Token::GetText() const noexcept
    {
        return m_Text;
    }

    std::string Token::GetTextString() const noexcept
    {
        return std::string(m_Text.data(), m_Text.length());
    }

    std::string Token::DebugInfo() const noexcept
    {
        std::string pos_info = "(" + std::to_string(GetLineNumber().get()) + ":" +
                               std::to_string(GetColumn().get()) + ")";

        switch (m_Type)
        {
            case Type::Colon:
                return "Token[Colon]" + pos_info;
            case Type::Comma:
                return "Token[Comma]" + pos_info;
            case Type::Comment:
                return "Token[Comment]" + pos_info + ": '" + GetTextString() + "'";
            case Type::Identifier:
                return "Token[Identifier]" + pos_info + ": '" + GetTextString() + "'";
            case Type::NewLine:
                return "Token[NewLine]" + pos_info;
            case Type::OpenBracket:
                return "Token[OpenBracket]" + pos_info;
            case Type::ClosingBracket:
                return "Token[OpenBracket]" + pos_info;
            case Type::IntegerLiteral:
                return "Token[IntegerLiteral]" + pos_info + ": '" + GetTextString() + "'";
            case Type::ImmediateInteger:
                return "Token[ImmediateInteger]" + pos_info + ": " + GetTextString() + "'";
            case Type::Unknown:
                return "Token[Unknown]" + pos_info;
            default:
                PHI_ASSERT_NOT_REACHED();
                return "Token[NotFound]" + pos_info;
        }
    }
} // namespace dlx
