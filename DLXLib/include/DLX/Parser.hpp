#pragma once

#include "DLX/ParsedProgram.hpp"
#include "DLX/Token.hpp"
#include "DLX/TokenStream.hpp"

namespace dlx
{
    class Parser
    {
    public:
        static ParsedProgram Parse(TokenStream& tokens) noexcept;

        static ParsedProgram Parse(phi::string_view source) noexcept;
    };
} // namespace dlx
