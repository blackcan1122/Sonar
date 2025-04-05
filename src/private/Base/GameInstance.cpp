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
	LOG_INFO("GameInstace Initialized");

}

std::string GameInstance::RegisterAsset(const std::string name)
{
	std::string FutureName = GenerateNextAvaiableName(name);
	std::string base;
	int num;
	if (!ParseAssetName(FutureName, base, num))
	{
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

	return FutureName;
}

void GameInstance::UnregisterAsset(const std::string name)
{
	std::string base;
	int num;
	if (!ParseAssetName(name, base, num)) return;

	auto it = AssetRegistry.find(base);
	if (it == AssetRegistry.end()) return;

	if (num == -1) num = 0; // Handle base name removal

	it->second.erase(num);
	if (it->second.empty()) {
		AssetRegistry.erase(it);
	}
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
		std::cerr << "GameInstance was already initialized" << std::endl;
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
	std::cerr << "GameInstance was not Created, please Call InitGameInstance first" << std::endl;
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
			ActiveStateMachine.KillLastGameMode();
		}
		ActiveStateMachine.UpdateGameMode();

		// Windows Resize Event
		if (GetScreenHeight() != GameInstance::GetInstance()->m_WindowProperties.ScreenHeight || GetScreenWidth() != GameInstance::GetInstance()->m_WindowProperties.ScreenWidth)
		{
			std::cout << "Window Resize" << std::endl;
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

