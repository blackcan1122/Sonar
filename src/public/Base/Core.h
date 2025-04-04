#pragma once
// Raylib

#include "raylib.h"
#include "raygui.h"
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
void init_logger();
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

#include "Object.hpp"

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


// MACROS


#define AUTOBODY(cls) \
public: \
    static std::type_index StaticClass() { return typeid(cls); }

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



// Logging Macros
#define LOG_INFO(...)  SPDLOG_LOGGER_INFO(g_logger, __VA_ARGS__)
#define LOG_WARN(...)  SPDLOG_LOGGER_WARN(g_logger, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(g_logger, __VA_ARGS__)