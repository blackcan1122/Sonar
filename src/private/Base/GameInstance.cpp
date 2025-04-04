#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif // !RAYGUI_IMPLEMENTATION
#include "Base/GameInstance.h"
#include "Base/Core.h"
#include "Base/StateMachine.h"
#include "Base/GameMode.h"
#include "UI/TextBox.h"
#include "Events/SaveGameEvent.h"
#include "Events/LoadGameEvent.h"
#include "Base/EventDispatcher.hpp"
#include "UI/Button.h"

// GameModes
#include "GameModes/SandboxGameMode.hpp"
#include "GameModes/Menu.hpp"
#include "GameModes/OptionsMode.hpp"

// Events
#include "Events/AllPurposeEvent.h"
#include "Events/UIEvent.h"

// EventData
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

EventDispatcher GameInstance::UIEventDispatcher;
EventDispatcher GameInstance::SaveStateDispatcher;
EventDispatcher GameInstance::AllPurposeDispatcher;
GameModeSwitcher GameInstance::ActiveStateMachine;
std::string GameInstance::WorkingDirectory;

// Definition of the static member
GameInstance* GameInstance::Instance = nullptr;

GameInstance::GameInstance(WindowProperties Properties)
	: m_WindowProperties(Properties)
{
	init_logger();
	spdlog::flush_every(std::chrono::seconds(1));
	LOG_INFO("GameInstace Initialized");

}


void GameInstance::InitGameInstance(WindowProperties Properties)
{
	if (Instance != nullptr)
	{
		LOG_ERROR("GameInstance already Existing");
		return;
	}

	WorkingDirectory = GetWorkingDirectory();

	Instance = new GameInstance(Properties);
	CreateWindow();
	GameLoop();
}

GameInstance* GameInstance::GetInstance()
{
	if (Instance != nullptr)
	{
		return Instance;
	}
	LOG_ERROR("Call InitGameInstance First");
}

GameMode* GameInstance::GetCurrentGameMode()
{
	return ActiveStateMachine.GetCurrentGameMode();
}

EventDispatcher& GameInstance::GetUIEventDispatcher()
{
	return UIEventDispatcher;
}

EventDispatcher& GameInstance::GetSaveStateEventDispatcher()
{
	return SaveStateDispatcher;
}


void GameInstance::CreateWindow()
{
	SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

	InitWindow(Instance->m_WindowProperties.ScreenWidth, Instance->m_WindowProperties.ScreenHeight, "Sonar");
	SetTargetFPS(Instance->m_WindowProperties.TargetFps);
}

void GameInstance::GameLoop()
{
	

	ActiveStateMachine.RegisterState("Menu", []() {return new MenuMode(); });
	ActiveStateMachine.RegisterState("Sandbox", []() {return new SandboxGameMode(); });
	ActiveStateMachine.RegisterState("Options", []() {return new OptionsMode(); });
	//ActiveStateMachine.RegisterState("Pong", []() {return new PongGameMod(); });
	//ActiveStateMachine.RegisterState("Chat", []() {return new ChatTest(); });


	// Setting initial Start Mode
	ActiveStateMachine.ChangeState("Menu");


	std::shared_ptr<AllPurposeEvent> WindowResizeEvent = std::make_shared<AllPurposeEvent>();
	std::shared_ptr<WindowResizeData> CurrentWindowResizeData = std::make_shared<WindowResizeData>();


	// GAMELOOP //
	while (!WindowShouldClose())
	{
		BeginDrawing();
		if (ActiveStateMachine.isPendingKillLastMode())
		{
			std::string LastName = ActiveStateMachine.GetLastGameMode()->GetName();
			ActiveStateMachine.KillLastGameMode();

		}

		ActiveStateMachine.UpdateGameMode();

		// Windows Resize Event
		if (GetScreenHeight() != GameInstance::GetInstance()->m_WindowProperties.ScreenHeight || GetScreenWidth() != GameInstance::GetInstance()->m_WindowProperties.ScreenWidth)
		{

			GameInstance::GetInstance()->m_WindowProperties.ScreenHeight = GetScreenHeight();
			GameInstance::GetInstance()->m_WindowProperties.ScreenWidth = GetScreenWidth();

			// Dispatch Event

			CurrentWindowResizeData->width = GetScreenWidth();
			CurrentWindowResizeData->height = GetScreenHeight();

			std::shared_ptr<IEventData> Payload = std::static_pointer_cast<IEventData>(CurrentWindowResizeData);

			WindowResizeEvent->Payload = Payload;

			AllPurposeDispatcher.Dispatch(WindowResizeEvent);

		}
		// GameMode Independend UI Drawings



		EndDrawing();
	}
}
