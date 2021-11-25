#pragma once

#include "DLX/TokenStream.hpp"
#include "ParsedProgram.hpp"
#include "Token.hpp"
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
