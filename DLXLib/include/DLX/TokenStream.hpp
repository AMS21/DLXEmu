#pragma once

#include "Token.hpp"
#include <Phi/Core/Assert.hpp>
#include <Phi/Core/Boolean.hpp>
#include <Phi/Core/Types.hpp>
#include <optional>
#include <utility>
#include <vector>

namespace dlx
{
    class TokenStream
    {
    public:
        using storage_type   = std::vector<Token>;
        using iterator       = typename storage_type::iterator;
        using const_iterator = typename storage_type::const_iterator;

        template <typename... ArgsT>
        void emplace_back(ArgsT&&... args) noexcept
        {
#if defined(PHI_DEBUG)
            PHI_DBG_ASSERT(!m_Finialized);
#endif

            m_Tokens.emplace_back(std::forward<ArgsT>(args)...);
        }

        void push_back(const Token& value) noexcept;

        void push_back(Token&& value) noexcept;

        void finalize() noexcept;

        void reset() noexcept;

        [[nodiscard]] phi::Boolean has_x_more(phi::usize x) const noexcept;

        [[nodiscard]] phi::Boolean has_more() const noexcept;

        [[nodiscard]] phi::Boolean reached_end() const noexcept;

        [[nodiscard]] Token& look_ahead() noexcept;

        [[nodiscard]] const Token& look_ahead() const noexcept;

        [[nodiscard]] Token& consume() noexcept;

        void skip(phi::usize n = 1u) noexcept;

        [[nodiscard]] Token* find_first_token_of_type(Token::Type type) noexcept;

        [[nodiscard]] const Token* find_first_token_of_type(Token::Type type) const noexcept;

        [[nodiscard]] Token* find_last_token_of_type(Token::Type type) noexcept;

        [[nodiscard]] const Token* find_last_token_of_type(Token::Type type) const noexcept;

        template <typename PredicateT>
        [[nodiscard]] Token* find_first_token_if(PredicateT pred) noexcept
        {
#if defined(PHI_DEBUG)
            PHI_ASSERT(m_Finialized);
#endif
            for (Token& token : m_Tokens)
            {
                if (pred(token))
                {
                    return &token;
                }
            }

            return nullptr;
        }

        template <typename PredicateT>
        [[nodiscard]] const Token* find_first_token_if(PredicateT pred) const noexcept
        {
#if defined(PHI_DEBUG)
            PHI_ASSERT(m_Finialized);
#endif

            for (const Token& token : m_Tokens)
            {
                if (pred(token))
                {
                    return &token;
                }
            }

            return nullptr;
        }

        // Complexity O(n)
        [[nodiscard]] phi::usize size() const noexcept;

        [[nodiscard]] phi::Boolean empty() const noexcept;

        // Iterator
        [[nodiscard]] iterator current_position() noexcept;

        [[nodiscard]] const_iterator current_position() const noexcept;

        void set_position(iterator it) noexcept;

        [[nodiscard]] iterator begin() noexcept;

        [[nodiscard]] const_iterator begin() const noexcept;

        [[nodiscard]] const_iterator cbegin() const noexcept;

        [[nodiscard]] iterator end() noexcept;

        [[nodiscard]] const_iterator end() const noexcept;

        [[nodiscard]] const_iterator cend() const noexcept;

    private:
        storage_type m_Tokens;
        iterator     m_Iterator = m_Tokens.begin();
#if defined(PHI_DEBUG)
        phi::Boolean m_Finialized{false};
#endif
    };
} // namespace dlx
