#pragma once
#include "Base/Export.hpp"

#include <cstdio>
#include <cstdarg>
#include <string>
#include <format>
#define SONAR_FMT(...) std::format(__VA_ARGS__)
SONARENGINE_API void InitLogger();
SONARENGINE_API void LogInfo(const std::string& Message);
SONARENGINE_API void LogInfo(const char* Category, const std::string& Message);
SONARENGINE_API void LogWarn(const std::string& Message);
SONARENGINE_API void LogWarn(const char* Category, const std::string& Message);
SONARENGINE_API void LogError(const std::string& Message);
SONARENGINE_API void LogError(const char* Category, const std::string& Message);

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

#define LOG_INFO(...) LogInfo(__VA_ARGS__)
#define LOG_WARN(...) LogWarn(__VA_ARGS__)
#define LOG_ERROR(...) LogError(__VA_ARGS__)