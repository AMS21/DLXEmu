#pragma once

#include <phi/compiler_support/warning.hpp>
#include <phi/phi_config.hpp>

#if defined(DLX_NO_LOG)
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#elif defined(PHI_DEBUG)
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
#endif

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")

#include <phi/preprocessor/function_like_macro.hpp>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

PHI_GCC_SUPPRESS_WARNING_POP()

namespace dlx
{
    bool InitializeDefaultLogger() noexcept;

    [[nodiscard]] spdlog::logger* GetLogger() noexcept;
} // namespace dlx

#define DLX_TRACE(...)                                                                             \
    PHI_BEGIN_MACRO()                                                                              \
    if (::dlx::GetLogger())                                                                        \
    {                                                                                              \
        SPDLOG_LOGGER_TRACE(::dlx::GetLogger(), __VA_ARGS__);                                      \
    }                                                                                              \
    PHI_END_MACRO()
#define DLX_DEBUG(...)                                                                             \
    PHI_BEGIN_MACRO()                                                                              \
    if (::dlx::GetLogger())                                                                        \
    {                                                                                              \
        SPDLOG_LOGGER_DEBUG(::dlx::GetLogger(), __VA_ARGS__);                                      \
    }                                                                                              \
    PHI_END_MACRO()
#define DLX_INFO(...)                                                                              \
    PHI_BEGIN_MACRO()                                                                              \
    if (::dlx::GetLogger())                                                                        \
    {                                                                                              \
        SPDLOG_LOGGER_INFO(::dlx::GetLogger(), __VA_ARGS__);                                       \
    }                                                                                              \
    PHI_END_MACRO()
#define DLX_WARN(...)                                                                              \
    PHI_BEGIN_MACRO()                                                                              \
    if (::dlx::GetLogger())                                                                        \
    {                                                                                              \
        SPDLOG_LOGGER_WARN(::dlx::GetLogger(), __VA_ARGS__);                                       \
    }                                                                                              \
    PHI_END_MACRO()
#define DLX_ERROR(...)                                                                             \
    PHI_BEGIN_MACRO()                                                                              \
    if (::dlx::GetLogger())                                                                        \
    {                                                                                              \
        SPDLOG_LOGGER_ERROR(::dlx::GetLogger(), __VA_ARGS__);                                      \
    }                                                                                              \
    PHI_END_MACRO()
#define DLX_CRITICAL(...)                                                                          \
    PHI_BEGIN_MACRO()                                                                              \
    if (::dlx::GetLogger())                                                                        \
    {                                                                                              \
        SPDLOG_LOGGER_CRITICAL(::dlx::GetLogger(), __VA_ARGS__);                                   \
    }                                                                                              \
    PHI_END_MACRO()
