#include <DLX/OpCode.hpp>
#include <DLX/RegisterNames.hpp>
#include <Phi/Core/Log.hpp>
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

    file_log->flush();

    return 0;
}
