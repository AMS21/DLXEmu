#include "DLX/TokenStream.hpp"

#include <Phi/Core/Assert.hpp>

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

        m_Iterator = m_Tokens.begin();
#if defined(PHI_DEBUG)
        m_Finialized = true;
#endif
    }

    void TokenStream::reset() noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Iterator = m_Tokens.begin();
    }

    phi::Boolean TokenStream::has_x_more(phi::usize x) const noexcept
    {
        auto it = m_Iterator;
        for (; it != m_Tokens.end() && x > 0u; ++it, --x)
        {}

        return it != m_Tokens.end();
    }

    phi::Boolean TokenStream::has_more() const noexcept
    {
        return m_Iterator != m_Tokens.end();
    }

    phi::Boolean TokenStream::reached_end() const noexcept
    {
        return m_Iterator == m_Tokens.end();
    }

    Token& TokenStream::look_ahead() noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return *m_Iterator;
    }

    const Token& TokenStream::look_ahead() const noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return *m_Iterator;
    }

    Token& TokenStream::consume() noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        return *m_Iterator++;
    }

    void TokenStream::skip(phi::usize n) noexcept
    {
        PHI_ASSERT(!reached_end());
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif
        PHI_ASSERT(has_x_more(n));
        PHI_DBG_ASSERT(n != 0u);

        while (n-- > 0u)
        {
            ++m_Iterator;
        }
    }

    Token* TokenStream::find_first_token_of_type(Token::Type type) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        for (Token& token : m_Tokens)
        {
            if (token.GetType() == type)
            {
                return &token;
            }
        }

        return nullptr;
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

    Token* TokenStream::find_last_token_of_type(Token::Type type) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        Token* last = nullptr;

        for (Token& token : m_Tokens)
        {
            if (token.GetType() == type)
            {
                last = &token;
            }
        }

        return last;
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

    [[nodiscard]] phi::Boolean TokenStream::empty() const noexcept
    {
        return m_Tokens.empty();
    }

    [[nodiscard]] TokenStream::iterator TokenStream::current_position() noexcept
    {
        return m_Iterator;
    }

    [[nodiscard]] TokenStream::const_iterator TokenStream::current_position() const noexcept
    {
        return m_Iterator;
    }

    void TokenStream::set_position(TokenStream::iterator it) noexcept
    {
#if defined(PHI_DEBUG)
        PHI_ASSERT(m_Finialized);
#endif

        m_Iterator = it;
    }

    TokenStream::iterator TokenStream::begin() noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.begin();
    }

    TokenStream::const_iterator TokenStream::begin() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.begin();
    }

    TokenStream::const_iterator TokenStream::cbegin() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.cbegin();
    }

    TokenStream::iterator TokenStream::end() noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.end();
    }

    TokenStream::const_iterator TokenStream::end() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.end();
    }

    TokenStream::const_iterator TokenStream::cend() const noexcept
    {
#if defined(PHI_DEBUG)
        //PHI_ASSERT(m_Finialized);
#endif

        return m_Tokens.cend();
    }
} // namespace dlx
