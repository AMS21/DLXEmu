#include "DLX/Logger.hpp"

#include <phi/core/assert.hpp>
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <iostream>

static constexpr const char DefaultFormattingString[]{
        "%^[%Y-%m-%d %H:%M:%S.%e] %n [%L](%s:%#):%$ %v"};

namespace dlx
{
    static bool is_initialized{false};

    bool InitializeDefaultLogger() noexcept
    {
        PHI_DBG_ASSERT(!is_initialized, "Logger is already initialized");

        try
        {
            std::shared_ptr<spdlog::logger> console = spdlog::stdout_color_mt("");
            spdlog::set_default_logger(console);
            spdlog::set_pattern(DefaultFormattingString);

            // In debug mode always flush
#if defined(PHI_DEBUG)
            spdlog::flush_on(spdlog::level::trace);
#else
            spdlog::flush_on(spdlog::level::warn);
#endif

            is_initialized = true;

            return true;
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            std::cerr << "Failed to initalize logger!\n";
            std::cerr << "spdlog message: " << ex.what() << '\n';
            return false;
        }
    }

    spdlog::logger* GetLogger() noexcept
    {
        return spdlog::default_logger_raw();
    }
} // namespace dlx
