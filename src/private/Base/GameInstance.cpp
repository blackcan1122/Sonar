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
std::unordered_map<std::string, std::set<int32_t>> GameInstance::AssetRegistry;

// Definition of the static member
GameInstance* GameInstance::Instance = nullptr;

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
		AssetRegistry[base].insert(0);
	}
	else 
	{
		AssetRegistry[base].insert(num);
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

	auto it = AssetRegistry.find(base);
	if (it == AssetRegistry.end())
	{
		LOG_ERROR(l_ASSET_REGISTRY, TEXT("Couldn't find Asset in AssetRegistry: '{}'", name));
		return false;
	}

	if (num == -1) num = 0; // Handle base name removal

	it->second.erase(num);
	if (it->second.empty()) {
		AssetRegistry.erase(it);
	}
	LOG_INFO(l_ASSET_REGISTRY, TEXT("Successfully removed '{}' from Asset Registry", name));
}

std::string GameInstance::GenerateNextAvaiableName(const std::string base_name)
{
	const auto& numbers = AssetRegistry[base_name];
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

void GameInstance::InitGameInstance(WindowProperties Properties)
{
	if (Instance != nullptr)
	{
		LOG_ERROR(l_GAME_INSTANCE, "It was tried to initialize a GameInstance, when GameInstace is already initialized");
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

	LOG_ERROR(l_GAME_INSTANCE, "GameInstace not Initialized, please Call: 'GameInstance::InitGameInstance(WindowProperties Properties)' first");
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

	LOG_INFO(l_GAME_INSTANCE, TEXT("Window Initialized with Properties, Size: {} x {}, TargetFPS: {}",
		Instance->m_WindowProperties.ScreenWidth, 
		Instance->m_WindowProperties.ScreenHeight, 
		Instance->m_WindowProperties.TargetFps));
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
	LOG_INFO(l_GAMEMODE, TEXT("Startup GameMode loaded: '{}'", ActiveStateMachine.GetCurrentGameMode()->GetName()));


	std::shared_ptr<AllPurposeEvent> WindowResizeEvent = std::make_shared<AllPurposeEvent>();
	std::shared_ptr<WindowResizeData> CurrentWindowResizeData = std::make_shared<WindowResizeData>();


	// GAMELOOP //
	while (!WindowShouldClose())
	{
		BeginDrawing();
		if (ActiveStateMachine.isPendingKillLastMode())
		{
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

