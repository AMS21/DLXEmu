#pragma once

#include "DLX/OpCode.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>
#include <string>

namespace dlx
{

#define DLX_ENUM_TOKEN_TYPE                                                                        \
    DLX_ENUM_TOKEN_TYPE_IMPL(Comment)                                                              \
    DLX_ENUM_TOKEN_TYPE_IMPL(OpCode)                                                               \
    DLX_ENUM_TOKEN_TYPE_IMPL(LabelIdentifier)                                                      \
    DLX_ENUM_TOKEN_TYPE_IMPL(RegisterInt)                                                          \
    DLX_ENUM_TOKEN_TYPE_IMPL(RegisterFloat)                                                        \
    DLX_ENUM_TOKEN_TYPE_IMPL(RegisterStatus)                                                       \
    DLX_ENUM_TOKEN_TYPE_IMPL(Comma)                                                                \
    DLX_ENUM_TOKEN_TYPE_IMPL(Colon)                                                                \
    DLX_ENUM_TOKEN_TYPE_IMPL(OpenBracket)                                                          \
    DLX_ENUM_TOKEN_TYPE_IMPL(ClosingBracket)                                                       \
    DLX_ENUM_TOKEN_TYPE_IMPL(NewLine)                                                              \
    DLX_ENUM_TOKEN_TYPE_IMPL(ImmediateInteger)                                                     \
    DLX_ENUM_TOKEN_TYPE_IMPL(IntegerLiteral)                                                       \
    DLX_ENUM_TOKEN_TYPE_IMPL(Unknown)

    class Token
    {
    public:
        enum class Type
        {
#define DLX_ENUM_TOKEN_TYPE_IMPL(name) name,
            DLX_ENUM_TOKEN_TYPE
#undef DLX_ENUM_TOKEN_TYPE_IMPL

                    COUNT_OF_TOKENS,
        };

    public:
        Token(Type type, phi::string_view text, phi::u64 line_number, phi::u64 column) noexcept;

        Token(Type type, phi::string_view text, phi::u64 line_number, phi::u64 column,
              std::uint32_t hint) noexcept;

        [[nodiscard]] Type GetType() const noexcept;

        [[nodiscard]] phi::string_view GetTypeName() const noexcept;

        [[nodiscard]] phi::u64 GetLineNumber() const noexcept;

        [[nodiscard]] phi::u64 GetColumn() const noexcept;

        [[nodiscard]] phi::usize GetLength() const noexcept;

        [[nodiscard]] phi::string_view GetText() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

        [[nodiscard]] std::string GetTextString() const noexcept;

        [[nodiscard]] phi::boolean HasHint() const noexcept;

        [[nodiscard]] std::uint32_t GetHint() const noexcept;

        [[nodiscard]] std::string DebugInfo() const noexcept;

        PHI_GCC_SUPPRESS_WARNING_POP()

    private:
        Type             m_Type;
        phi::string_view m_Text;
        phi::u64         m_LineNumber;
        phi::u64         m_Column;
        std::uint32_t    m_Hint;
        phi::boolean     m_HasHint;
    };

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wreturn-type")

    template <>
    [[nodiscard]] constexpr phi::string_view enum_name<Token::Type>(Token::Type value) noexcept
    {
        switch (value)
        {
#define DLX_ENUM_TOKEN_TYPE_IMPL(name)                                                             \
    case Token::Type::name:                                                                        \
        return #name;

            DLX_ENUM_TOKEN_TYPE

#undef DLX_ENUM_TOKEN_TYPE_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

} // namespace dlx
