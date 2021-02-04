#pragma once

#include "DLX/OpCode.hpp"
#include <string>
#include <string_view>
#include <Phi/Core/Types.hpp>

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
        Token(Type type, std::string_view text, phi::u32 line_number, phi::u32 column);

        [[nodiscard]] Type GetType() const noexcept;

        [[nodiscard]] std::string_view GetTypeName() const noexcept;

        [[nodiscard]] phi::u32 GetLineNumber() const noexcept;

        [[nodiscard]] phi::u32 GetColumn() const noexcept;

        [[nodiscard]] phi::usize GetLength() const noexcept;

        [[nodiscard]] std::string_view GetText() const noexcept;

        [[nodiscard]] std::string GetTextString() const noexcept;

        [[nodiscard]] std::string DebugInfo() const noexcept;

    private:
        Type             m_Type;
        std::string_view m_Text;
        phi::u32         m_LineNumber;
        phi::u32         m_Column;
    };
} // namespace dlx
