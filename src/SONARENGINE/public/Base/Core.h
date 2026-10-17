#pragma once
// Raylib

#include "Rendering/Graphics.hpp"

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
#include <utility>


#include "Base/Logging.h"

#if defined(_WIN32)           // raylib uses these names as function parameters
#undef near
#undef far
#endif

#define nullpath SoftObjectPath<IObject>()

#define FTOI(x) static_cast<int>(x)
#define ITOF(x) static_cast<float>(x)


// Own Class Forward
//class EventDispatcher;
//class Factory;
//class StateMachine;
//class GameMode;
//class GameInstance;
//class TextInputBox;
//class Button;
//class IEvent;
//class AssetRegistry;
//class World;
//template <typename T>
//struct SoftObjectPath;


// Own Header includes, for very important headers
#include "Base/Object.hpp"
#include "Base/Macros.h"
//#include "Base/SoftObject.hpp"

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
