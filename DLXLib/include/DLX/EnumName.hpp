#pragma once

#include <phi/compiler_support/warning.hpp>
#include <phi/container/string_view.hpp>
#include <phi/type_traits/false_t.hpp>

PHI_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")

namespace dlx
{
    template <typename EnumT>
    [[nodiscard]] constexpr phi::string_view enum_name(EnumT /*value*/) noexcept
    {
        static_assert(phi::false_v<EnumT>, "dlx::enum_name was not specified for the given enum");

        return "";
    }
} // namespace dlx
