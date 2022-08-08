#include "DLX/Logger.hpp"

#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <iostream>

static constexpr const char DefaultFormattingString[]{
        "%^[%Y-%m-%d %H:%M:%S.%e] %n [%L](%s:%#):%$ %v"};

namespace dlx
{
    static phi::boolean is_initialized{false};

    phi::boolean InitializeDefaultLogger() noexcept
    {
        PHI_ASSERT(!is_initialized, "Logger is already initialized");

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

    spdlog::logger* GetLogger() noexcept
    {
        return spdlog::default_logger_raw();
    }
} // namespace dlx
