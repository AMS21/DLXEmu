#include "DLX/Logger.hpp"
#include <DLX/OpCode.hpp>
#include <DLX/RegisterNames.hpp>
#include <magic_enum.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
    // Create log file
    std::shared_ptr<spdlog::logger> file_log;

    try
    {
        file_log = spdlog::basic_logger_st("file_logger", "Dictionary.txt", true);

        file_log->set_level(spdlog::level::trace);
        file_log->flush_on(spdlog::level::err);
        file_log->set_pattern("%v");
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return -1;
    }

    // Write all opcodes
    for (const auto& opcode : magic_enum::enum_names<dlx::OpCode>())
    {
        // Skip non tokens
        if (opcode == "NONE" || opcode == "NUMBER_OF_ELEMENTS")
        {
            continue;
        }

        file_log->info(R"(opcode_{0}="{0}")", opcode);
    }

    // Write all int registers
    for (const auto& reg : magic_enum::enum_names<dlx::IntRegisterID>())
    {
        // Skip none registers
        if (reg == "None")
        {
            continue;
        }

        file_log->info(R"(int_register_id_{0}="{0}")", reg);
    }

    // Write all float registers
    for (const auto& reg : magic_enum::enum_names<dlx::FloatRegisterID>())
    {
        // Skip none registers
        if (reg == "None")
        {
            continue;
        }

        file_log->info(R"(float_register_id_{0}="{0}")", reg);
    }

    // Write FPSR
    file_log->info(R"(FPSR="FPSR")");

    // Write normal tokens
    file_log->info(R"(space=" ")");
    file_log->info(R"(comma=",")");
    file_log->info(R"(colon=":")");
    file_log->info(R"(new_line="\x0a")");
    file_log->info(R"(open_bracket="(")");
    file_log->info(R"t(closing_bracket=")")t");
    file_log->info(R"(hash="#")");
    file_log->info(R"(underscore="_")");
    file_log->info(R"(forward_slash="/")");
    file_log->info(R"(semicolon=";")");

    // Write numbers
    file_log->info(R"(num0="0")");
    file_log->info(R"(num1="1")");
    file_log->info(R"(num2="2")");
    file_log->info(R"(num3="3")");
    file_log->info(R"(num4="4")");
    file_log->info(R"(num5="5")");
    file_log->info(R"(num6="6")");
    file_log->info(R"(num7="7")");
    file_log->info(R"(num8="8")");
    file_log->info(R"(num9="9")");
    file_log->info(R"(hex10="A")");
    file_log->info(R"(hex11="B")");
    file_log->info(R"(hex12="C")");
    file_log->info(R"(hex13="D")");
    file_log->info(R"(hex14="E")");
    file_log->info(R"(hex15="F")");

    file_log->info(R"(hex_begin="0x")");
    file_log->info(R"(bin_begin="0b")");

    file_log->info(R"(plus="+")");
    file_log->info(R"(minus="-")");

    file_log->flush();

    return 0;
}
