#pragma once

#include "TokenStream.hpp"

namespace dlx
{
    [[nodiscard]] TokenStream Tokenize(phi::string_view source) noexcept;
}
