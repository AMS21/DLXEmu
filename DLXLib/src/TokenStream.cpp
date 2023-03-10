#include "DLX/TokenStream.hpp"

#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/types.hpp>

PHI_GCC_SUPPRESS_WARNING("-Wsuggest-attribute=pure")

namespace dlx
{
    void TokenStream::push_back(const Token& value) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(!m_Finialized);
#endif

        m_Tokens.push_back(value);
    }

    void TokenStream::push_back(Token&& value) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(!m_Finialized);
#endif

        m_Tokens.push_back(value);
    }

    void TokenStream::finalize() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(!m_Finialized);
#endif

        m_Iterator = 0u;
#if defined(PHI_DEBUG)
        m_Finialized = true;
#endif
    }

    void TokenStream::reset() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Iterator = 0u;
    }

    phi::boolean TokenStream::has_x_more(phi::usize x) const noexcept
    {
        return x + m_Iterator <= m_Tokens.size();
    }

    phi::boolean TokenStream::has_more() const noexcept
    {
        return m_Iterator < m_Tokens.size();
    }

    phi::boolean TokenStream::reached_end() const noexcept
    {
        return m_Iterator >= m_Tokens.size();
    }

    const Token& TokenStream::look_ahead() const noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.at(m_Iterator.unsafe());
    }

    const Token& TokenStream::consume() noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.at(m_Iterator++.unsafe());
    }

    void TokenStream::skip(phi::usize n) noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif
        PHI_ASSERT(has_x_more(n));
        PHI_ASSERT(n != 0u);

        for (; n > 0u; --n, ++m_Iterator)
        {}
    }

    const Token* TokenStream::find_first_token_of_type(Token::Type type) const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        for (const Token& token : m_Tokens)
        {
            if (token.GetType() == type)
            {
                return &token;
            }
        }

        return nullptr;
    }

    const Token* TokenStream::find_last_token_of_type(Token::Type type) const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        const Token* last = nullptr;

        for (const Token& token : m_Tokens)
        {
            if (token.GetType() == type)
            {
                last = &token;
            }
        }

        return last;
    }

    phi::usize TokenStream::size() const noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif
        phi::usize count{0u};

        for (auto it = m_Tokens.begin(); it != m_Tokens.end(); ++it)
        {
            count += 1u;
        }

        return count;
    }

    PHI_ATTRIBUTE_CONST phi::boolean TokenStream::empty() const noexcept
    {
        return m_Tokens.empty();
    }

    PHI_ATTRIBUTE_CONST phi::usize TokenStream::current_position() const noexcept
    {
        return m_Iterator;
    }

    void TokenStream::set_position(phi::usize pos) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Iterator = pos;
    }

    PHI_ATTRIBUTE_CONST TokenStream::const_iterator TokenStream::begin() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.begin();
    }

    PHI_ATTRIBUTE_CONST TokenStream::const_iterator TokenStream::cbegin() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.cbegin();
    }

    PHI_ATTRIBUTE_CONST TokenStream::const_iterator TokenStream::end() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.end();
    }

    PHI_ATTRIBUTE_CONST TokenStream::const_iterator TokenStream::cend() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.cend();
    }

    PHI_ATTRIBUTE_CONST TokenStream::const_reverse_iterator TokenStream::rbegin() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.rbegin();
    }

    TokenStream::const_reverse_iterator TokenStream::rend() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.rend();
    }

    PHI_ATTRIBUTE_CONST const Token& TokenStream::front() const noexcept
    {
        return m_Tokens.front();
    }

    PHI_ATTRIBUTE_CONST const Token& TokenStream::back() const noexcept
    {
        return m_Tokens.back();
    }
} // namespace dlx
