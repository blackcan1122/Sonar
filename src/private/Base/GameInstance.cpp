#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif // !RAYGUI_IMPLEMENTATION

#define Version "DEV-0.1.1"

#include "Base/Core.h"

// GameInstance Specific
#include "Base/GameInstance.h"
#include "Base/StateMachine.h"
#include "Base/AssetRegistry.hpp"

// UI
#include "UI/Button.h"
#include "UI/TextBox.h"

// GameModes
#include "Base/GameMode.h"
#include "GameModes/SandboxGameMode.hpp"
#include "GameModes/Menu.hpp"
#include "GameModes/OptionsMode.hpp"

// Event System
#include "Base/EventDispatcher.hpp"

// Events
#include "Events/AllPurposeEvent.h"
#include "Events/UIEvent.h"
#include "Events/SaveGameEvent.h"
#include "Events/LoadGameEvent.h"

// EventData
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

// Definition of the static member
EventDispatcher GameInstance::UIEventDispatcher;
EventDispatcher GameInstance::SaveStateDispatcher;
EventDispatcher GameInstance::AllPurposeDispatcher;

GameModeSwitcher GameInstance::g_ActiveStateMachine;

GameThreadQueue GameInstance::MainQueue;
ResourceManager GameInstance::g_ResourceManager;
std::shared_ptr<AssetRegistry> GameInstance::g_AssetRegistry = std::make_shared<AssetRegistry>();

std::string GameInstance::g_WorkingDirectory;
GameInstance* GameInstance::g_Instance = nullptr;

GameInstance::GameInstance(WindowProperties Properties)
	: m_WindowProperties(Properties)
{
	InitLogger();
	spdlog::flush_every(std::chrono::milliseconds(10));
	LOG_INFO(l_GAME_INSTANCE, TEXT("GameInstance Initialized"));
	
	SetTraceLogCallback(&GameInstance::RedirectTraceLog);
}

TextureResource* GameInstance::GetResource(std::string Name)
{
	try
	{
		return &(g_ResourceManager.AllResources.at(Name));
	}
	catch (std::out_of_range e)
	{
		std::cerr << e.what() << std::endl;
		return nullptr;
	}

}

void GameInstance::InitGameInstance(WindowProperties Properties)
{
	if (g_Instance != nullptr)
	{
		LOG_ERROR(l_GAME_INSTANCE, "It was tried to initialize a GameInstance, when GameInstace is already initialized");
		return;
	}

	g_WorkingDirectory = GetWorkingDirectory();

	g_Instance = new GameInstance(Properties);
	CreateWindow();
	GameLoop();
}

GameInstance* GameInstance::GetInstance()
{
	if (g_Instance != nullptr)
	{
		return g_Instance;
	}

	LOG_ERROR(l_GAME_INSTANCE, "GameInstace not Initialized, please Call: 'GameInstance::InitGameInstance(WindowProperties Properties)' first");
}

GameMode* GameInstance::GetCurrentGameMode()
{
	return g_ActiveStateMachine.GetCurrentGameMode();
}

std::shared_ptr<AssetRegistry> GameInstance::GetAssetRegistry()
{
	return g_AssetRegistry;
}

EventDispatcher& GameInstance::GetUIEventDispatcher()
{
	return UIEventDispatcher;
}

EventDispatcher& GameInstance::GetSaveStateEventDispatcher()
{
	return SaveStateDispatcher;
}

void GameInstance::RedirectTraceLog(int logLevel, const char* text, va_list args)
{
	char buffer[1024];
	FORMAT_VA(buffer, text, args);

	if (logLevel <= 3)
	{

		LOG_INFO(l_RAYLIB, buffer);
	}
	else if (logLevel == 4)
	{
		LOG_WARN(l_RAYLIB, buffer);
	}
	else
	{
		LOG_ERROR(l_RAYLIB, buffer);
	}
	
}


void GameInstance::CreateWindow()
{
	SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

	InitWindow(g_Instance->m_WindowProperties.m_ScreenWidth, g_Instance->m_WindowProperties.m_ScreenHeight, "Sonar");
	SetTargetFPS(g_Instance->m_WindowProperties.m_TargetFps);

	LOG_INFO(l_GAME_INSTANCE, TEXT("Window Initialized with Properties, Size: {} x {}, TargetFPS: {}",
		g_Instance->m_WindowProperties.m_ScreenWidth, 
		g_Instance->m_WindowProperties.m_ScreenHeight, 
		g_Instance->m_WindowProperties.m_TargetFps));
}

void GameInstance::GameLoop()
{
	
	g_ResourceManager.ParseJson();
	g_ActiveStateMachine.RegisterState("Menu", []() {return new MenuMode(); });
	g_ActiveStateMachine.RegisterState("Sandbox", []() {return new SandboxGameMode(); });
	g_ActiveStateMachine.RegisterState("Options", []() {return new OptionsMode(); });
	//ActiveStateMachine.RegisterState("Pong", []() {return new PongGameMod(); });
	//ActiveStateMachine.RegisterState("Chat", []() {return new ChatTest(); });


	// Setting initial Start Mode
	g_ActiveStateMachine.ChangeState("Menu");
	LOG_INFO(l_GAMEMODE, TEXT("Startup GameMode loaded: '{}'", g_ActiveStateMachine.GetCurrentGameMode()->GetName()));


	std::shared_ptr<AllPurposeEvent> WindowResizeEvent = std::make_shared<AllPurposeEvent>();
	std::shared_ptr<WindowResizeData> CurrentWindowResizeData = std::make_shared<WindowResizeData>();



	// GAMELOOP //
	while (!WindowShouldClose())
	{
		MainQueue.ProcessTasks();
		BeginDrawing();
		if (g_ActiveStateMachine.isPendingKillLastMode())
		{
			g_ActiveStateMachine.KillLastGameMode();
		}
		g_ActiveStateMachine.UpdateGameMode();

		// Windows Resize Event
		if (GetScreenHeight() != GameInstance::GetInstance()->m_WindowProperties.m_ScreenHeight || GetScreenWidth() != GameInstance::GetInstance()->m_WindowProperties.m_ScreenWidth)
		{
			GameInstance::GetInstance()->m_WindowProperties.m_ScreenHeight = GetScreenHeight();
			GameInstance::GetInstance()->m_WindowProperties.m_ScreenWidth = GetScreenWidth();

			// Dispatch Event

			CurrentWindowResizeData->width = GetScreenWidth();
			CurrentWindowResizeData->height = GetScreenHeight();

			std::shared_ptr<IEventData> Payload = std::static_pointer_cast<IEventData>(CurrentWindowResizeData);

			WindowResizeEvent->Payload = Payload;

			AllPurposeDispatcher.Dispatch(WindowResizeEvent);

		}

		// GameMode Independend UI Drawings

		DrawText((std::string("Version: ") + Version).c_str(), 0 + 5, GetScreenHeight() - 20, 12, WHITE);
		EndDrawing();
	}
}

