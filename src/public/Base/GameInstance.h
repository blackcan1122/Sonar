#pragma once
#include "Base/Core.h"
#include "Base/EventDispatcher.hpp"
#include "Base/StateMachine.h"
#include "StateMachines/GameModeSwitcher.h"

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

public:

	GameInstance(const GameInstance&) = delete;
	GameInstance& operator=(const GameInstance&) = delete;

	static EventDispatcher UIEventDispatcher;
	static EventDispatcher SaveStateDispatcher;
	static EventDispatcher AllPurposeDispatcher;

	static GameModeSwitcher ActiveStateMachine;

	static std::string WorkingDirectory;

	static void InitGameInstance(WindowProperties Properties);
	static GameInstance* GetInstance();

	WindowProperties GetWindowProperties() const { return m_WindowProperties; }

	static GameMode* GetCurrentGameMode();


	static EventDispatcher& GetUIEventDispatcher();
	static EventDispatcher& GetSaveStateEventDispatcher();


};