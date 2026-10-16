#pragma once

#include <cstdio>
#include <cstdarg>
#include <memory>
#include <string>

#if defined(_WIN32)
#define NOGDI
#define NOUSER
#endif

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#define SPDLOG_ENABLE_SOURCE_LOC 1

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

inline std::shared_ptr<spdlog::logger> GeneralLogger = nullptr;

void InitLogger();

// Project-local formatting helper. Avoid TEXT because Windows headers define it too.
#define SONAR_FMT(text, ...) fmt::format(text, ##__VA_ARGS__)

#define FORMAT_VA(buffer, fmt, args) \
    do { va_list _copy; va_copy(_copy, args); vsnprintf(buffer, sizeof(buffer), fmt, _copy); va_end(_copy); } while(0)

#define l_DEFAULT "[DEFAULT] "
#define l_GAMEMODE "[GAMEMODE] "
#define l_ASSET_REGISTRY "[ASSET REGISTRY] "
#define l_GAME_INSTANCE "[GAME INSTANCE] "
#define l_FACTORY "[IOBJECT FACTORY] "
#define l_DISPATCHER "[EVENT DISPATCHER] "
#define l_HOUSE_KEEPING "[HOUSE KEEPING] "
#define l_RESOURCES "[RESOURCES] "
#define l_RAYLIB "[RAYLIB] "

inline void LogMessage(spdlog::level::level_enum Level, const char* Category, const std::string& Message)
{
    if (!GeneralLogger)
    {
        return;
    }

    GeneralLogger->log(Level, "{} {}", Category, Message);
}

inline void LogMessage(spdlog::level::level_enum Level, const std::string& Message)
{
    LogMessage(Level, l_DEFAULT, Message);
}

inline void LogInfo(const std::string& Message)
{
    LogMessage(spdlog::level::info, Message);
}

inline void LogInfo(const char* Category, const std::string& Message)
{
    LogMessage(spdlog::level::info, Category, Message);
}

inline void LogWarn(const std::string& Message)
{
    LogMessage(spdlog::level::warn, Message);
}

inline void LogWarn(const char* Category, const std::string& Message)
{
    LogMessage(spdlog::level::warn, Category, Message);
}

inline void LogError(const std::string& Message)
{
    LogMessage(spdlog::level::err, Message);
}

inline void LogError(const char* Category, const std::string& Message)
{
    LogMessage(spdlog::level::err, Category, Message);
}

#define LOG_INFO(...)  LogInfo(__VA_ARGS__)
#define LOG_WARN(...)  LogWarn(__VA_ARGS__)
#define LOG_ERROR(...) LogError(__VA_ARGS__)