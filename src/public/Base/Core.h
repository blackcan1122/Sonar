#pragma once
// Raylib

#include "raylib.h"
#include "raygui.hpp"
#include "rlgl.h"
#include "raymath.h"

// C++ Std
#include <functional>
#include <memory>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <iostream>
#include <typeindex>
#include <chrono>
#include <filesystem>


// Spdlog

#if defined(_WIN32)           
#define NOGDI             // All GDI defines and routines
#define NOUSER            // All USER defines and routines
#endif

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO  // Set minimal log level to capture
#define SPDLOG_ENABLE_SOURCE_LOC 1             // Enable source location

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
inline std::shared_ptr<spdlog::logger> g_logger = nullptr;
void InitLogger();
#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif



// Own
class EventDispatcher;
class StateMachine;
class GameMode;
class GameInstance;
class TextInputBox;
class Button;
class IEvent;

#include "Base/Object.hpp"

// Unsure if i should keep them away, as handling recursive includes suck
//#include "Base/TickableFactory.h"
//#include "TickAll.h"
//#include "Tickable.h"

//#include "Base/Event.hpp"
//#include "Base/EventDispatcher.hpp"
//#include "Physic.hpp"
//#include "Base/Helper.hpp"
//#include "Base/StateMachine.h"
//#include "Base/GameMode.h"
//#include "GameInstance.h"

// STRUCTS


/**
* SoftObjectPath provides a way to reference objects by name rather than direct pointer,
* allowing for lazy loading, improved serialization, and better memory management.
*/
template <typename T>
struct SoftObjectPath
{

    SoftObjectPath<T>::SoftObjectPath() {};

    SoftObjectPath(std::string SoftPath)
        :Path(SoftPath)
    {
    }

    std::shared_ptr<T> TryLoad()
    {
        return GameInstance::GetInstance()->LoadAssetFromSoftObjectPath(*this);
    }

    std::string ToString()
    {
        return Path;
    }

private:
    std::string Path;
};


// MACROS


#define AUTOBODY(cls) \
public: \
    static std::type_index StaticClass() { return typeid(cls); } \
private:

// Macro for intermediate CRTP base classes
#define DECLARE_CRTP_INTERMEDIATE_CLASS(ClassName, BaseName) \
template <typename Derived> \
class ClassName : public BaseName<Derived>

// Macro for final leaf classes
#define DECLARE_CRTP_LEAF_CLASS(ClassName, BaseName) \
class ClassName : public BaseName<ClassName>

#define DECLARE_CLASS(ClassName, BaseClass) \
class ClassName : public BaseClass \
{ \
public: \
AUTOBODY(ClassName) \

#define END_CLASS };



#define TEXT(text, ...) fmt::format(text, ##__VA_ARGS__)

// Logging Categories
#define l_DEFAULT "[DEFAULT] "
#define l_GAMEMODE "[GAMEMODE] "
#define l_ASSET_REGISTRY "[ASSET REGISTRY] "
#define l_GAME_INSTANCE "[GAME INSTANCE] "
#define l_FACTORY "[IOBJECT FACTORY] "
#define l_DISPATCHER "[EVENT DISPATCHER] "
#define l_HOUSE_KEEPING "[HOUSE KEEPING] "
#define l_RESOURCES "[RESOURCES] "

inline void LogInfo(const std::string& message) {
    SPDLOG_LOGGER_INFO(g_logger, "{} {}", l_DEFAULT, message);
}

inline void LogInfo(const char* cat, const std::string& message) {
    SPDLOG_LOGGER_INFO(g_logger, "{} {}", cat, message);
}

inline void LogWarn(const std::string& message) {
    SPDLOG_LOGGER_WARN(g_logger, "{} {}", l_DEFAULT, message);
}

inline void LogWarn(const char* cat, const std::string& message) {
    SPDLOG_LOGGER_WARN(g_logger, "{} {}", cat, message);
}

inline void LogError(const std::string& message) {
    SPDLOG_LOGGER_ERROR(g_logger, "{} {}", l_DEFAULT, message);
}

inline void LogError(const char* cat, const std::string& message) {
    SPDLOG_LOGGER_ERROR(g_logger, "{} {}", cat, message);
}


// Logging Macros
#define LOG_INFO(...)  LogInfo(__VA_ARGS__)
#define LOG_WARN(...)  LogWarn(__VA_ARGS__)
#define LOG_ERROR(...) LogError(__VA_ARGS__)

