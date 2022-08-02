#include <DLX/OpCode.hpp>
#include <DLX/RegisterNames.hpp>
#include <phi/container/array.hpp>
#include <phi/core/scope_guard.hpp>
#include <spdlog/fmt/bundled/os.h>
#include <spdlog/fmt/compile.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>
#include <cstdio>
#include <iostream>
#include <memory>

int main()
{
    fmt::ostream out = fmt::output_file("Dictionary.txt");

    static const constexpr phi::array opcodes{
#define DLX_ENUM_OPCODE_IMPL(name) #name,
            DLX_ENUM_OPCODE
#undef DLX_ENUM_OPCODE_IMPL
    };

    static const constexpr phi::array int_regs{
#define DLX_ENUM_INT_REGISTER_ID_IMPL(name) #name,
            DLX_ENUM_INT_REGISTER_ID
#undef DLX_ENUM_INT_REGISTER_ID_IMPL
    };

    static const constexpr phi::array float_regs{
#define DLX_ENUM_FLOAT_REGISTER_ID_IMPL(name) #name,
            DLX_ENUM_FLOAT_REGISTER_ID
#undef DLX_ENUM_FLOAT_REGISTER_ID_IMPL
    };

    // Write all opcodes
    for (const auto& opcode : opcodes)
    {
        out.print(R"(opcode_{0}="{0}")", opcode);
    }

    // Write all int registers
    for (const auto& reg : int_regs)
    {
        out.print(R"(int_register_id_{0}="{0}")", reg);
    }

    // Write all float registers
    for (const auto& reg : float_regs)
    {
        out.print(R"(float_register_id_{0}="{0}")", reg);
    }

    // Write FPSR
    out.print(R"(FPSR="FPSR")");

    // Write normal tokens
    out.print(R"(space=" ")");
    out.print(R"(comma=",")");
    out.print(R"(colon=":")");
    out.print(R"(new_line="\x0a")");
    out.print(R"(open_bracket="(")");
    out.print(R"t(closing_bracket=")")t");
    out.print(R"(hash="#")");
    out.print(R"(underscore="_")");
    out.print(R"(forward_slash="/")");
    out.print(R"(semicolon=";")");

    // Write numbers
    out.print(R"(num0="0")");
    out.print(R"(num1="1")");
    out.print(R"(num2="2")");
    out.print(R"(num3="3")");
    out.print(R"(num4="4")");
    out.print(R"(num5="5")");
    out.print(R"(num6="6")");
    out.print(R"(num7="7")");
    out.print(R"(num8="8")");
    out.print(R"(num9="9")");
    out.print(R"(hex10="A")");
    out.print(R"(hex11="B")");
    out.print(R"(hex12="C")");
    out.print(R"(hex13="D")");
    out.print(R"(hex14="E")");
    out.print(R"(hex15="F")");

    out.print(R"(hex_begin="0x")");
    out.print(R"(bin_begin="0b")");

    out.print(R"(plus="+")");
    out.print(R"(minus="-")");

    return 0;
}
