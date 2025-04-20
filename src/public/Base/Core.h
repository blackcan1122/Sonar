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
inline std::shared_ptr<spdlog::logger> GeneralLogger = nullptr;
void InitLogger();
#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif



// Own Class Forward
class EventDispatcher;
class StateMachine;
class GameMode;
class GameInstance;
class TextInputBox;
class Button;
class IEvent;

// Own Header includes, for very important headers
#include "Base/Object.hpp"
#include "Base/NavalTypedefs.h"

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
    SoftObjectPath() = default;


    // Implicit upcast constructor (from derived to base)
    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    SoftObjectPath(const SoftObjectPath<U>& other) : Path(other.Path) {}

    // Explicit cast method for related types
    template <typename U>
    SoftObjectPath<U> Cast() const 
    {
        static_assert(std::is_base_of_v<IObject, U>, "U must derive from IObject");
        static_assert(std::is_base_of_v<U, T> || std::is_base_of_v<T, U>,
            "Types must be related by inheritance");
        return SoftObjectPath<U>(Path);
    }

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


// Autobody Macro for implementing a Static function in every IObject Derived Class, to make it possible to compare Static Classes against
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

// Macro for all IObject Derived Classes, to correctly implement the Autobody
#define DECLARE_CLASS(ClassName, BaseClass) \
class ClassName : public BaseClass \
{ \
public: \
AUTOBODY(ClassName) \


#define END_CLASS };


// Text macro, to parse variadic arguments into a finished std::string
#define TEXT(text, ...) fmt::format(text, ##__VA_ARGS__)

#define FORMAT_VA(buffer, fmt, args) \
    do { va_list _copy; va_copy(_copy, args); vsnprintf(buffer, sizeof(buffer), fmt, _copy); va_end(_copy); } while(0)


// Logging Categories
#define l_DEFAULT "[DEFAULT] "
#define l_GAMEMODE "[GAMEMODE] "
#define l_ASSET_REGISTRY "[ASSET REGISTRY] "
#define l_GAME_INSTANCE "[GAME INSTANCE] "
#define l_FACTORY "[IOBJECT FACTORY] "
#define l_DISPATCHER "[EVENT DISPATCHER] "
#define l_HOUSE_KEEPING "[HOUSE KEEPING] "
#define l_RESOURCES "[RESOURCES] "
#define l_RAYLIB "[RAYLIB] "

// Log Functions
inline void LogInfo(const std::string& Message) 
{
    SPDLOG_LOGGER_INFO(GeneralLogger, "{} {}", l_DEFAULT, Message);
}

inline void LogInfo(const char* Category, const std::string& Message)
{
    SPDLOG_LOGGER_INFO(GeneralLogger, "{} {}", Category, Message);
}

inline void LogWarn(const std::string& Message)
{
    SPDLOG_LOGGER_WARN(GeneralLogger, "{} {}", l_DEFAULT, Message);
}

inline void LogWarn(const char* Category, const std::string& Message)
{
    SPDLOG_LOGGER_WARN(GeneralLogger, "{} {}", Category, Message);
}

inline void LogError(const std::string& Message)
{
    SPDLOG_LOGGER_ERROR(GeneralLogger, "{} {}", l_DEFAULT, Message);
}

inline void LogError(const char* Category, const std::string& Message)
{
    SPDLOG_LOGGER_ERROR(GeneralLogger, "{} {}", Category, Message);
}


// Logging Macros
#define LOG_INFO(...)  LogInfo(__VA_ARGS__)
#define LOG_WARN(...)  LogWarn(__VA_ARGS__)
#define LOG_ERROR(...) LogError(__VA_ARGS__)

