#pragma once

#include <phi/compiler_support/warning.hpp>
#include <phi/type_traits/false_t.hpp>
#include <string_view>

PHI_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")

namespace dlx
{
    template <typename EnumT>
    [[nodiscard]] constexpr std::string_view enum_name(EnumT /*value*/) noexcept
    {
        static_assert(phi::false_v<EnumT>, "dlx::enum_name was not specified for the given enum");

        return "";
    }
} // namespace dlx
