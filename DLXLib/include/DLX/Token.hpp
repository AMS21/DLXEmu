#pragma once

#include "DLX/OpCode.hpp"
#include <Phi/Core/Types.hpp>
#include <string>
#include <string_view>

namespace dlx
{
    class Token
    {
    public:
        enum class Type
        {
            Comment,
            Identifier,
            Comma,
            Colon,
            OpenBracket,
            ClosingBracket,
            NewLine,
            ImmediateInteger,
            IntegerLiteral,

            Unknown,
        };

    public:
        Token(Type type, std::string_view text, phi::u64 line_number, phi::u64 column);

        [[nodiscard]] Type GetType() const noexcept;

        [[nodiscard]] std::string_view GetTypeName() const noexcept;

        [[nodiscard]] phi::u64 GetLineNumber() const noexcept;

        [[nodiscard]] phi::u64 GetColumn() const noexcept;

        [[nodiscard]] phi::usize GetLength() const noexcept;

        [[nodiscard]] std::string_view GetText() const noexcept;

        [[nodiscard]] std::string GetTextString() const noexcept;

        [[nodiscard]] std::string DebugInfo() const noexcept;

    private:
        Type             m_Type;
        std::string_view m_Text;
        phi::u64         m_LineNumber;
        phi::u64         m_Column;
    };
} // namespace dlx
