#pragma once

#include "TokenStream.hpp"

namespace dlx
{
    [[nodiscard]] TokenStream Tokenize(std::string_view source) noexcept;
}
