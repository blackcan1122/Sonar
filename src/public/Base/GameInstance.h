#pragma once
#include "Base/Core.h"
#include <algorithm>
#include "Base/EventDispatcher.hpp"
#include "Base/ResourceManager.hpp"
#include "Base/StateMachine.h"
#include "StateMachines/GameModeSwitcher.h"
#include "Base/GameThreadQueue.hpp"
#include "Base/GameMode.h"
#include "Base/AssetRegistry.hpp"

struct WindowProperties
{
	WindowProperties(int ScreenWidth, int ScreenHeight, int TargetFps, bool Fullscreen, bool IsDebug)
		: m_ScreenWidth(ScreenWidth), m_ScreenHeight(ScreenHeight), m_TargetFps(TargetFps), m_Fullscreen(Fullscreen), m_IsDebug(IsDebug)
	{
	}

	int m_ScreenWidth;
	int m_ScreenHeight;
	int m_TargetFps;
	bool m_Fullscreen;
	bool m_IsDebug = false;
};


/**
 * @class GameInstance
 * @brief Manages the core systems, lifecycle, and multithreaded behavior of the game, including asset management and mode switching.
 *
 * Purpose:
 * - Acts as the central singleton for handling game-wide systems and behavior.
 * - Manages the game window, active game modes, event dispatching, and resource loading.
 * - Provides multithreaded mechanisms for asset management and event handling.
 *
 * Key Responsibilities:
 * - **Game Lifecycle**:
 *   - Manages the initialization and main game loop through `InitGameInstance()` and `GameLoop()`.
 * - **Window Management**:
 *   - Stores and manages window properties like resolution, fullscreen mode, and target FPS.
 * - **Event Dispatching**:
 *   - Provides `UIEventDispatcher`, `SaveStateDispatcher`, and `AllPurposeDispatcher` for decoupled event communication across systems.
 * - **Asset Management**:
 *   - Facilitates the loading, registration, and unloading of assets via the asset registry and `ResourceManager`.
 *   - Supports `SoftObjectPath` to enable lazy loading and safer asset referencing.
 * - **Game Mode Switching**:
 *   - Switches between game modes using the `StateMachine` and tracks the current active mode.

 * Multithreading Behavior:
 * - **ResourceManager Integration**:
 *   - Works closely with the `ResourceManager` to load assets asynchronously.
 *   - Uses `std::future` for asynchronous tasks, ensuring non-blocking behavior during loading.
 *   - Maintains thread safety by leveraging atomic operations for asset reference counting and worker thread management.
 * - **Asset Registry**:
 *   - Stores and tracks registered assets in a thread-safe manner.
 *   - Uses unique asset names to prevent conflicts and facilitate lazy loading via `SoftObjectPath`.
 *   - Interacts with the active `GameMode` to check and retrieve assets during runtime.
 * - **SoftObjectPath**:
 *   - Works in conjunction with `SoftObjectPath` to resolve asset references at runtime.
 *   - Ensures that assets are only loaded into memory when needed, improving memory efficiency.
 *   - Validates asset availability by checking the current game mode and asset registry.

 * Usage:
	 1. Initialize the game instance:
		GameInstance::InitGameInstance(WindowProperties(parameters));
 
	 2. Use `GetInstance()` to retrieve the global instance for accessing features like :
	-`GetCurrentGameMode()` to access the active game mode.
	 -`LoadAssetFromSoftObjectPath()` to safely load assets.
	 -`GetUIEventDispatcher()` to send UI - related events.
	 3. Register assets explicitly using `RegisterAsset()` and ensure proper cleanup with `UnregisterAsset()`.

	 Notes for Developers:
	-**Thread Safety * *:
		 -Ensure that resource loading and asset registration are handled carefully to avoid race conditions.
		  -Use provided methods like `LoadAssetFromSoftObjectPath()` or directly Call `TryLoad()` on the SoftObjectPath Object to ensure safe asset access.
		  -**Asset Registry * *:
	-Avoid modifying the asset registry directly; rely on helper methods for consistent behavior.
		  -Asset names must be unique and follow a consistent naming convention.
		  -**SoftObjectPath * *:
	-Use `SoftObjectPath` for referencing objects by name instead of direct pointers.
		  -Ensure that assets referenced by `SoftObjectPath` exist in the registry to avoid runtime errors. <-- this can be provided, when created via factory
		  -**Game Mode Switching * *:
	-When switching game modes, ensure that all assets and objects from the previous mode are properly cleaned up.
	-When using std::shared_ptr or SoftObjectPath and SharedTexure2D this will happen automatically, when you allocate memory yourself, you are also responsible for it
		  -**Event Dispatching * *:
		 -Use the provided dispatchers for decoupled communication, avoiding direct dependencies between systems.
		 
*/

class GameInstance
{

private:
	GameInstance(WindowProperties Properties);
	~GameInstance() = default;

	static GameInstance* g_Instance;

protected:

	/*
	*****************************
	* Window and Start Parameter
	*****************************
	*/

	WindowProperties m_WindowProperties;

	/*
	*****************************
	* Methods
	*****************************
	*/

	static ResourceManager g_ResourceManager;
	static AssetRegistry g_AssetRegistry;

	static void CreateWindow();
	static void GameLoop();


public:

	GameInstance(const GameInstance&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;

	static EventDispatcher UIEventDispatcher;
	static EventDispatcher SaveStateDispatcher;
	static EventDispatcher AllPurposeDispatcher;

	static GameThreadQueue MainQueue;

	TextureResource* GetResource(std::string Name);


	/*
	*****************************
	* Utitlity
	*****************************
	*/

	static GameModeSwitcher g_ActiveStateMachine;

	static std::string g_WorkingDirectory;

	static void InitGameInstance(WindowProperties Properties);
	static GameInstance* GetInstance();

	WindowProperties GetWindowProperties() const { return m_WindowProperties; }

	static GameMode* GetCurrentGameMode();
	static AssetRegistry* GetAssetRegistry();


	static EventDispatcher& GetUIEventDispatcher();
	static EventDispatcher& GetSaveStateEventDispatcher();

	static void RedirectTraceLog(int logLevel, const char* text, va_list args);


};