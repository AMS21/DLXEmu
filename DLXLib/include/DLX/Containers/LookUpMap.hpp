#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>

namespace dlx
{
    template <typename KeyT, typename ValueT, std::size_t SizeT>
    class LookUpMap
    {
    public:
        constexpr LookUpMap(const std::array<std::pair<KeyT, ValueT>, SizeT>& data,
                            ValueT default_value) noexcept
            : m_Data(data)
            , m_Default(default_value)
        {}

        [[nodiscard]] constexpr ValueT at(const KeyT& key) const
        {
            const auto itr = std::find_if(begin(m_Data), end(m_Data),
                                          [&key](const auto& v) { return v.first == key; });

            if (itr != end(m_Data))
            {
                return itr->second;
            }

            return m_Default;
        }

    private:
        std::array<std::pair<KeyT, ValueT>, SizeT> m_Data;
        ValueT                                     m_Default;
    };
} // namespace dlx
