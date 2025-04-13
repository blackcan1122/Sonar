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
GameModeSwitcher GameInstance::g_ActiveStateMachine;
GameThreadQueue GameInstance::MainQueue;
ResourceManager GameInstance::m_ResourceManager;

std::string GameInstance::g_WorkingDirectory;
std::unordered_map<std::string, std::set<int32_t>> GameInstance::g_AssetRegistry;

// Definition of the static member
GameInstance* GameInstance::g_Instance = nullptr;

GameInstance::GameInstance(WindowProperties Properties)
	: m_WindowProperties(Properties)
{
	InitLogger();
	spdlog::flush_every(std::chrono::seconds(1));
	LOG_INFO(l_GAME_INSTANCE, TEXT("GameInstance Initialized"));
}

std::string GameInstance::RegisterAsset(const std::string name)
{
	std::string FutureName = GenerateNextAvaiableName(name);
	std::string base;
	int num;
	if (!ParseAssetName(FutureName, base, num))
	{
		LOG_ERROR(l_ASSET_REGISTRY, TEXT("Error Parsing AssetName: '{}'", FutureName));
		return std::string("");
	}

	if (num == -1) 
	{
		// Base name without number (implicit 0)
		g_AssetRegistry[base].insert(0);
	}
	else 
	{
		g_AssetRegistry[base].insert(num);
	}

	LOG_INFO(l_ASSET_REGISTRY, TEXT("'{}' Added to Asset Registry", FutureName));
	return FutureName;
}

bool GameInstance::UnregisterAsset(const std::string name)
{
	std::string base;
	int num;
	if (!ParseAssetName(name, base, num))
	{
		LOG_ERROR(l_ASSET_REGISTRY, TEXT("Problem with Unregistering Asset: '{}'", name));
		return false;
	}

	auto it = g_AssetRegistry.find(base);
	if (it == g_AssetRegistry.end())
	{
		LOG_ERROR(l_ASSET_REGISTRY, TEXT("Couldn't find Asset in AssetRegistry: '{}'", name));
		return false;
	}

	if (num == -1) num = 0; // Handle base name removal

	it->second.erase(num);
	if (it->second.empty()) {
		g_AssetRegistry.erase(it);
	}
	LOG_INFO(l_ASSET_REGISTRY, TEXT("Successfully removed '{}' from Asset Registry", name));
}

std::string GameInstance::GenerateNextAvaiableName(const std::string base_name)
{
	const auto& numbers = g_AssetRegistry[base_name];
	if (numbers.empty()) {
		return base_name; // Use base name first
	}

	// Find first gap starting from 0
	int expected = 0;
	for (int num : numbers) 
	{
		if (num > expected) break;
		expected++;
	}

	if (expected == 0) 
	{
		return base_name; // Use base name if 0 is available
	}

	// Format with leading zero for 2-digit numbers
	std::ostringstream oss;
	oss << base_name << "_" << std::setw(2) << std::setfill('0') << expected;
	return oss.str();
}


bool GameInstance::ParseAssetName(const std::string& FullName, std::string& OutBaseName, int32_t& OutNumber)
{
	size_t last_underscore = FullName.find_last_of('_');
	if (last_underscore == FullName.npos)
	{
		OutBaseName = FullName;
		OutNumber = -1;
		return true;
	}

	std::string NumberPart = FullName.substr(last_underscore + 1);
	if (NumberPart.empty())
	{
		return false;
	}

	try
	{
		size_t Index;
		int Num = std::stoi(NumberPart, &Index);
		if (Index != NumberPart.size())
		{
			return false;
		}

		OutBaseName = FullName.substr(0, last_underscore);
		OutNumber = Num;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

TextureResource* GameInstance::GetResource(std::string Name)
{
	try
	{
		return &(m_ResourceManager.AllResources.at(Name));
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

	InitWindow(g_Instance->m_WindowProperties.m_ScreenWidth, g_Instance->m_WindowProperties.m_ScreenHeight, "Sonar");
	SetTargetFPS(g_Instance->m_WindowProperties.m_TargetFps);

	LOG_INFO(l_GAME_INSTANCE, TEXT("Window Initialized with Properties, Size: {} x {}, TargetFPS: {}",
		g_Instance->m_WindowProperties.m_ScreenWidth, 
		g_Instance->m_WindowProperties.m_ScreenHeight, 
		g_Instance->m_WindowProperties.m_TargetFps));
}

void GameInstance::GameLoop()
{
	
	m_ResourceManager.ParseJson();
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



		EndDrawing();
	}
}

