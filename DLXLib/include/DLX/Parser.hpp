#pragma once

#include "DLX/ParsedProgram.hpp"
#include "DLX/Token.hpp"
#include "DLX/TokenStream.hpp"
#include <string_view>
#include <vector>

namespace dlx
{
    class Parser
    {
    public:
        static ParsedProgram Parse(TokenStream& tokens) noexcept;

        static ParsedProgram Parse(std::string_view source) noexcept;
    };
} // namespace dlx
