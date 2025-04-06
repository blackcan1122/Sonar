#pragma once
#include "Base/Core.h"
#include "Base/EventDispatcher.hpp"
#include "Base/StateMachine.h"
#include "StateMachines/GameModeSwitcher.h"
#include <set>
#include <sstream>
#include <algorithm>

struct WindowProperties
{
	WindowProperties(int ScreenWidth, int ScreenHeight, int TargetFps, bool Fullscreen, bool IsDebug)
		: ScreenWidth(ScreenWidth), ScreenHeight(ScreenHeight), TargetFps(TargetFps), Fullscreen(Fullscreen), IsDebug(IsDebug)
	{
	}
	int ScreenWidth;
	int ScreenHeight;
	int TargetFps;
	bool Fullscreen;
	bool IsDebug = false;
};

class GameInstance
{

private:
	GameInstance(WindowProperties Properties);
	~GameInstance() = default;

	static GameInstance* Instance;
	static std::unordered_map<std::string, std::set<int32_t>> AssetRegistry;


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

	static void CreateWindow();
	static void GameLoop();

	bool ParseAssetName(const std::string& FullName, std::string& OutBaseName, int32_t& OutNumber);

public:

	GameInstance(const GameInstance&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;

	static EventDispatcher UIEventDispatcher;
	static EventDispatcher SaveStateDispatcher;
	static EventDispatcher AllPurposeDispatcher;

	/*
	*****************************
	* Asset Registry
	* INFO:
	* This could be refactored to reside inside the GameMode, cause in the current Implemention, only one GameMode can be active at a time
	* but for future use or expansion, i will let them here for now
	* but then it also should be its own class
	* 
	*****************************
	*/

	template <typename T>
	std::shared_ptr<T> LoadAssetFromSoftObjectPath(SoftObjectPath<T> Path)
	{
		std::string FullPath = Path.ToString();
		size_t Index = FullPath.find_first_of("/");

		std::string GameMode = FullPath.substr(0, Index);
		std::string Object = FullPath.substr(Index + 1);

		if (ActiveStateMachine.GetCurrentGameMode()->GetName() != GameMode)
		{
			return nullptr;
		}

		auto MapIT = ActiveStateMachine.GetCurrentGameMode()->m_Objects.find(FullPath);

		if (MapIT == ActiveStateMachine.GetCurrentGameMode()->m_Objects.end())
		{
			return nullptr;
		}

		std::shared_ptr<T> CastedOBJ = std::dynamic_pointer_cast<T>(MapIT->second);

		return CastedOBJ;
	}

	std::string RegisterAsset(const std::string name);

	void UnregisterAsset(const std::string name);

	std::string GenerateNextAvaiableName(const std::string base_name);

	/*
	*****************************
	* Utitlity
	*****************************
	*/

	static GameModeSwitcher ActiveStateMachine;

	static std::string WorkingDirectory;

	static void InitGameInstance(WindowProperties Properties);
	static GameInstance* GetInstance();

	WindowProperties GetWindowProperties() const { return m_WindowProperties; }

	static GameMode* GetCurrentGameMode();


	static EventDispatcher& GetUIEventDispatcher();
	static EventDispatcher& GetSaveStateEventDispatcher();


};