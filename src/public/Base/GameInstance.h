#pragma once
#include "Base/Core.h"
#include "Base/EventDispatcher.hpp"
#include "Base/StateMachine.h"
#include "StateMachines/GameModeSwitcher.h"
#include <set>
#include <sstream>
#include <algorithm>
#include "Base/GameMode.h"
#include "Base/ResourceManager.hpp"

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

class GameInstance
{

private:
	GameInstance(WindowProperties Properties);
	~GameInstance() = default;

	static GameInstance* g_Instance;
	static std::unordered_map<std::string, std::set<int32_t>> g_AssetRegistry;


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

	ResourceManager m_ResourceManager;

	static void CreateWindow();
	static void GameLoop();

	bool ParseAssetName(const std::string& FullName, std::string& OutBaseName, int32_t& OutNumber);

public:

	GameInstance(const GameInstance&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;

	static EventDispatcher UIEventDispatcher;
	static EventDispatcher SaveStateDispatcher;
	static EventDispatcher AllPurposeDispatcher;

	TextureResource* GetResource(std::string Name);

	/*
	*****************************
	* Asset Registry
	* INFO:
	* This could (and should) be refactored to its own Class
	* also this could reside inside the GameMode, cause in the current Implemention, only one GameMode can be active at a time
	* but for future use or expansion, i will let them seperate for now
	* but it still should be its own class
	* 
	*****************************
	*/

	template <typename T>
	inline std::shared_ptr<T> LoadAssetFromSoftObjectPath(SoftObjectPath<T> Path)
	{
		std::string FullPath = Path.ToString();
		size_t Index = FullPath.find_first_of("/");

		std::string GameMode = FullPath.substr(0, Index);
		std::string Object = FullPath.substr(Index + 1);

		if (g_ActiveStateMachine.GetCurrentGameMode()->GetName() != GameMode)
		{
			return nullptr;
		}

		auto MapIT = g_ActiveStateMachine.GetCurrentGameMode()->m_Objects.find(FullPath);

		if (MapIT == g_ActiveStateMachine.GetCurrentGameMode()->m_Objects.end())
		{
			return nullptr;
		}

		std::shared_ptr<T> CastedOBJ = std::dynamic_pointer_cast<T>(MapIT->second);

		return CastedOBJ;
	}

	std::string RegisterAsset(const std::string name);

	bool UnregisterAsset(const std::string name);

	std::string GenerateNextAvaiableName(const std::string base_name);

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


	static EventDispatcher& GetUIEventDispatcher();
	static EventDispatcher& GetSaveStateEventDispatcher();


};