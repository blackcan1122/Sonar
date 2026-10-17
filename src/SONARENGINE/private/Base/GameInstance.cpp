#include "Base/LoggingBackend.hpp"
#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#endif // !RAYGUI_IMPLEMENTATION

#define Version "DEV-0.1.5"

#include "Base/Core.h"

// GameInstance Specific
#include "Base/GameInstance.h"
#include "Base/GameMode.h"
#include "Base/StateMachine.h"
#include "Base/CrashHandler.hpp"
#include "Base/AssetRegistry.hpp"

// Event System
#include "Base/EventDispatcher.hpp"

// Events
#include "Events/AllPurposeEvent.h"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

// EventData
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

#include "Base/SoftObject.hpp"
#include "Base/AssetRegistry.hpp"

#include "Base/SClass.hpp"

std::shared_ptr<AssetRegistry> GetGlobalAssetRegistry()
{
	return GameInstance::GetAssetRegistry();
}

// Definition of the static member
EventDispatcher GameInstance::UIEventDispatcher;
EventDispatcher GameInstance::SaveStateDispatcher;
EventDispatcher GameInstance::AllPurposeDispatcher;
EventDispatcher GameInstance::KeyDispatcher;
EventDispatcher GameInstance::MouseDispatcher;

GameModeSwitcher GameInstance::g_ActiveStateMachine;

GameThreadQueue GameInstance::MainQueue;
ResourceManager GameInstance::g_ResourceManager;
std::shared_ptr<AssetRegistry> GameInstance::g_AssetRegistry = std::make_shared<AssetRegistry>();

std::string GameInstance::g_WorkingDirectory;
std::string GameInstance::g_ResourceRoot;
std::string GameInstance::g_StartupGameMode;
EngineRuntimeContext GameInstance::g_RuntimeContext;
GameInstance* GameInstance::g_Instance = nullptr;

GameInstance::GameInstance(WindowProperties Properties)
	: m_WindowProperties(Properties)
{
	InitLogger();
	InstallCrashHandler();
	spdlog::flush_every(std::chrono::milliseconds(10));
	LOG_INFO(l_GAME_INSTANCE, SONAR_FMT("GameInstance Initialized"));
	LOG_INFO(l_GAME_INSTANCE, SONAR_FMT("sizeof std::string: {}", (sizeof(std::string))));

	Sonar::SetTraceLogCallback(&GameInstance::RedirectTraceLog);
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

	g_WorkingDirectory = std::filesystem::current_path().generic_string();
	g_ResourceRoot = g_WorkingDirectory + "/resources";
	g_ResourceManager.SetWorkingDirectory(g_WorkingDirectory);
	g_ResourceManager.SetResourceRoot(g_ResourceRoot);
	g_ResourceManager.SetMainQueue(&MainQueue);
	g_RuntimeContext.Configure(
		&UIEventDispatcher,
		&KeyDispatcher,
		&MouseDispatcher,
		&SaveStateDispatcher,
		&AllPurposeDispatcher,
		&MainQueue,
		&g_WorkingDirectory,
		&g_ResourceRoot,
		[]() -> int
		{
			return GameInstance::GetInstance()->GetWindowProperties().m_ScreenWidth;
		},
		[]() -> int
		{
			return GameInstance::GetInstance()->GetWindowProperties().m_ScreenHeight;
		},
		[](const std::string& ResourceName) -> TextureResource*
		{
			return GameInstance::GetInstance()->GetResource(ResourceName);
		},
		[](const std::string& RelativePath) -> std::string
		{
			return GameInstance::g_ResourceManager.ResolveResourcePath(RelativePath);
		},
		[](const std::string& ShaderFileName) -> std::string
		{
			return GameInstance::g_ResourceManager.ResolveShaderPath(ShaderFileName);
		},
		[](const std::string& GameModeName)
		{
			GameInstance::GetInstance()->ChangeGameMode(GameModeName);
		});

	g_Instance = new GameInstance(Properties);
	CreateWindow();
	GameLoop();
}

void GameInstance::RegisterGameMode(const std::string& GameModeName, std::function<GameMode*()> FactoryFunction)
{
	g_ActiveStateMachine.RegisterState(GameModeName, [FactoryFunction = std::move(FactoryFunction)]()
		{
			auto* GameModeInstance = FactoryFunction();
			GameModeInstance->SetRuntimeContext(&g_RuntimeContext);
			return GameModeInstance;
		});
}

void GameInstance::SetStartupGameMode(const std::string& GameModeName)
{
	g_StartupGameMode = GameModeName;
}

EngineRuntimeContext& GameInstance::GetRuntimeContext()
{
	return g_RuntimeContext;
}

GameInstance* GameInstance::GetInstance()
{
	if (g_Instance != nullptr)
	{
		return g_Instance;
	}

	LOG_ERROR(l_GAME_INSTANCE, "GameInstace not Initialized, please Call: 'GameInstance::InitGameInstance(WindowProperties Properties)' first");
	return nullptr;
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
	Sonar::SetConfigFlags(Sonar::FLAG_WINDOW_HIGHDPI | Sonar::FLAG_MSAA_4X_HINT | Sonar::FLAG_WINDOW_RESIZABLE
	    | (g_Instance->m_WindowProperties.m_FrameLimit > 0 ? Sonar::FLAG_WINDOW_HIDDEN : 0));

	Sonar::InitWindow(g_Instance->m_WindowProperties.m_ScreenWidth, g_Instance->m_WindowProperties.m_ScreenHeight, "Sonar");
	Sonar::SetTargetFPS(g_Instance->m_WindowProperties.m_TargetFps);

	Sonar::InitAudioDevice();

	LOG_INFO(l_GAME_INSTANCE, SONAR_FMT("Window Initialized with Properties, Size: {} x {}, TargetFPS: {}",
		g_Instance->m_WindowProperties.m_ScreenWidth,
		g_Instance->m_WindowProperties.m_ScreenHeight,
		g_Instance->m_WindowProperties.m_TargetFps));
}

void GameInstance::GameLoop()
{
	g_ResourceManager.ParseJson();
	if (g_StartupGameMode.empty())
	{
		LOG_ERROR(l_GAME_INSTANCE, SONAR_FMT("No startup game mode registered before InitGameInstance()"));
		return;
	}

	// Setting initial Start Mode
	g_ActiveStateMachine.ChangeState(g_StartupGameMode);
	LOG_INFO(l_GAMEMODE, SONAR_FMT("Startup GameMode loaded: '{}'", g_ActiveStateMachine.GetCurrentGameMode()->GetName()));

	std::shared_ptr<AllPurposeEvent> WindowResizeEvent = std::make_shared<AllPurposeEvent>();
	std::shared_ptr<WindowResizeData> CurrentWindowResizeData = std::make_shared<WindowResizeData>();

	std::shared_ptr<KeyEvent> KeyEventDispatch = std::make_shared<KeyEvent>();
	std::shared_ptr<MouseEvent> MouseEventDispatch = std::make_shared<MouseEvent>();

	// GAMELOOP //
	int frameCount = 0;
	while (!Sonar::WindowShouldClose() && !GetRuntimeContext().m_ShouldShutdown.load()
	    && (g_Instance->m_WindowProperties.m_FrameLimit == 0 || frameCount++ < g_Instance->m_WindowProperties.m_FrameLimit))
	{
		int KeyPressed = Sonar::GetKeyPressed();
		if (KeyPressed != 0)
		{
			KeyEventDispatch->KeyPressed = static_cast<Sonar::KeyboardKey>(KeyPressed);
			KeyEventDispatch->MousePos = Sonar::GetMousePosition();

			KeyDispatcher.Dispatch(KeyEventDispatch, "GlobalKeyEvent");
		}

		if (Sonar::IsMouseButtonPressed(Sonar::MOUSE_BUTTON_LEFT))
		{
			MouseEventDispatch->KeyPressed = Sonar::MOUSE_BUTTON_LEFT;
			MouseEventDispatch->MousePos = Sonar::GetMousePosition();

			MouseDispatcher.Dispatch(MouseEventDispatch, "MouseGlobalEvent");
		}

		if (Sonar::IsMouseButtonPressed(Sonar::MOUSE_BUTTON_RIGHT))
		{
			MouseEventDispatch->KeyPressed = Sonar::MOUSE_BUTTON_RIGHT;
			MouseEventDispatch->MousePos = Sonar::GetMousePosition();

			MouseDispatcher.Dispatch(MouseEventDispatch, "MouseGlobalEvent");
		}

		if (Sonar::IsMouseButtonPressed(Sonar::MOUSE_BUTTON_MIDDLE))
		{
			MouseEventDispatch->KeyPressed = Sonar::MOUSE_BUTTON_MIDDLE;
			MouseEventDispatch->MousePos = Sonar::GetMousePosition();

			MouseDispatcher.Dispatch(MouseEventDispatch, "MouseGlobalEvent");
		}

		MainQueue.ProcessTasks();
		Sonar::BeginDrawing();
		if (g_ActiveStateMachine.isPendingKillLastMode())
		{
			g_ActiveStateMachine.KillLastGameMode();
		}
		g_ActiveStateMachine.UpdateGameMode();

		// Windows Resize Event
		if (Sonar::GetScreenHeight() != GameInstance::GetInstance()->m_WindowProperties.m_ScreenHeight || Sonar::GetScreenWidth() != GameInstance::GetInstance()->m_WindowProperties.m_ScreenWidth)
		{
			GameInstance::GetInstance()->m_WindowProperties.m_ScreenHeight = Sonar::GetScreenHeight();
			GameInstance::GetInstance()->m_WindowProperties.m_ScreenWidth = Sonar::GetScreenWidth();

			// Dispatch Event
			CurrentWindowResizeData->width = Sonar::GetScreenWidth();
			CurrentWindowResizeData->height = Sonar::GetScreenHeight();

			std::shared_ptr<IEventData> Payload = std::static_pointer_cast<IEventData>(CurrentWindowResizeData);

			WindowResizeEvent->Payload = Payload;

			AllPurposeDispatcher.Dispatch(WindowResizeEvent);
		}

		// GameMode Independend UI Drawings
		Sonar::DrawText((std::string("Version: ") + Version).c_str(), 0 + 5, Sonar::GetScreenHeight() - 20, 12, Sonar::WHITE);
		Sonar::EndDrawing();
	}

	LOG_INFO(l_GAME_INSTANCE, "Waiting for resource cleanup threads...");
	g_ActiveStateMachine.ShutdownCurrentGameMode();
	GameInstance::g_ResourceManager.CleanAllResources();

	LOG_INFO(l_GAME_INSTANCE, "Processing pending tasks...");
	MainQueue.ProcessTasks();

	LOG_INFO(l_GAME_INSTANCE, "Resources cleared");

	LOG_INFO(l_GAME_INSTANCE, "Closing audio and window...");

	Sonar::SetTraceLogCallback(nullptr);

	Sonar::CloseAudioDevice();
	Sonar::CloseWindow();

	LOG_INFO(l_GAME_INSTANCE, "GameInstance shutdown complete");

	spdlog::shutdown();
}

void GameInstance::ChangeGameMode(std::string GameModeName)
{
	MainQueue.Enqueue([RequestedGameMode = std::move(GameModeName)]()
		{
			GameInstance::g_ActiveStateMachine.ChangeState(RequestedGameMode);
		});
}