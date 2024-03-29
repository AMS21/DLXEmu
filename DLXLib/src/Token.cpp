#include "DLX/Token.hpp"

#include "DLX/EnumName.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <string>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <fmt/core.h>

PHI_GCC_SUPPRESS_WARNING_POP()

PHI_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")
PHI_CLANG_SUPPRESS_WARNING("-Wswitch-default")

namespace dlx
{
    Token::Token(Type type, phi::string_view text, phi::u64 line_number, phi::u64 column) noexcept
        : m_Type{type}
        , m_Text{text}
        , m_LineNumber{line_number}
        , m_Column{column}
        , m_Hint{0u}
        , m_HasHint{false}
    {}

    Token::Token(Type type, phi::string_view text, phi::u64 line_number, phi::u64 column,
                 phi::uint32_t hint) noexcept
        : m_Type{type}
        , m_Text{text}
        , m_LineNumber{line_number}
        , m_Column{column}
        , m_Hint{hint}
        , m_HasHint{true}
    {}

    Token::Type Token::GetType() const noexcept
    {
        return m_Type;
    }

    phi::string_view Token::GetTypeName() const noexcept
    {
        return dlx::enum_name(m_Type);
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

    phi::string_view Token::GetText() const noexcept
    {
        return m_Text;
    }

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

    std::string Token::GetTextString() const noexcept
    {
        return std::string(m_Text.data(), m_Text.length().unsafe());
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

    phi::boolean Token::HasHint() const noexcept
    {
        return m_HasHint;
    }

    phi::uint32_t Token::GetHint() const noexcept
    {
        PHI_ASSERT(m_Type == Type::RegisterInt || m_Type == Type::RegisterFloat ||
                   m_Type == Type::IntegerLiteral || m_Type == Type::OpCode ||
                   m_Type == Type::ImmediateInteger);
        PHI_ASSERT(m_HasHint);

        return m_Hint;
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wswitch")
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wreturn-type")

    std::string Token::DebugInfo() const noexcept
    {
        std::string pos_info =
                fmt::format("({:d}:{:d})", GetLineNumber().unsafe(), GetColumn().unsafe());

        switch (m_Type)
        {
            case Type::Colon:
                return "Token[Colon]" + pos_info;

            case Type::Comma:
                return "Token[Comma]" + pos_info;

            case Type::Comment:
                return "Token[Comment]" + pos_info + ": '" + GetTextString() + "'";

            case Type::OpCode:
                return "Token[OpCode]" + pos_info + ": '" + GetTextString() + "'";

            case Type::RegisterInt:
                return "Token[RegisterInt]" + pos_info + ": '" + GetTextString() + "'";

            case Type::RegisterFloat:
                return "Token[RegisterFloat]" + pos_info + ": '" + GetTextString() + "'";

            case Type::RegisterStatus:
                return "Token[RegisterStatus]" + pos_info + ": '" + GetTextString() + "'";

            case Type::LabelIdentifier:
                return "Token[LabelIdentifier]" + pos_info + ": '" + GetTextString() + "'";

            case Type::NewLine:
                return "Token[NewLine]" + pos_info;

            case Type::OpenBracket:
                return "Token[OpenBracket]" + pos_info;

            case Type::ClosingBracket:
                return "Token[ClosingBracket]" + pos_info;

            case Type::IntegerLiteral:
                return "Token[IntegerLiteral]" + pos_info + ": '" + GetTextString() + "'";

            case Type::ImmediateInteger:
                return "Token[ImmediateInteger]" + pos_info + ": " + GetTextString() + "'";

#if !defined(DLXEMU_COVERAGE_BUILD)
            case Type::Unknown:
                return "Token[Unknown]" + pos_info;

            default:
                PHI_ASSERT_NOT_REACHED();
                return "Token[NotFound]" + pos_info;
#endif
        }
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

} // namespace dlx
