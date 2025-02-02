#pragma once

#include <spdlog/spdlog.h>

#define SET_DEBUG_LOG_LEVEL spdlog::set_level(spdlog::level::debug)
#define SET_INFO_LOG_LEVEL spdlog::set_level(spdlog::level::info)
#define SET_WARN_LOG_LEVEL spdlog::set_level(spdlog::level::warn)
#define SET_ERROR_LOG_LEVEL spdlog::set_level(spdlog::level::err)
#define SET_CRITICAL_LOG_LEVEL spdlog::set_level(spdlog::level::critical)

#define LOG_DEBUG(message, ...) spdlog::debug(message __VA_OPT__(,) __VA_ARGS__)
#define LOG_INFO(message, ...) spdlog::info(message __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARN(message, ...) spdlog::warn(message __VA_OPT__(,) __VA_ARGS__)
#define LOG_ERROR(message, ...) spdlog::error(message __VA_OPT__(,) __VA_ARGS__)
#define LOG_CRITICAL(message, ...) spdlog::critical(message __VA_OPT__(,) __VA_ARGS__)
