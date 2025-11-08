#include "GameModes/Menu.hpp"
#include "Base/GameInstance.h"
#include "Base/StateMachine.h"
#include "Base/Factory.hpp"

// Events
#include "Events/AllPurposeEvent.h"
#include "Events/UIEvent.h"

// EventData
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

#define ButtonWidth 200
#define ButtonHeight 75

MenuMode::MenuMode()
{
	SetName("Menu");
}

MenuMode::~MenuMode()
{
	StopMusicStream(MenuMusic);
	UnloadMusicStream(MenuMusic);
	GameInstance::GetInstance()->AllPurposeDispatcher.RemoveListener("WindowsResize Menu", AllPurposeEvent::StaticClass());
}

void MenuMode::Update()
{
	ClearBackground(GREEN);
	DrawTexture(Background,0,0,WHITE);
	GameMode::Update();
	UpdateMusicStream(MenuMusic);
}

void MenuMode::BeginPlay()
{
	// Audio device should be initialized once at application startup, not per GameMode
	// InitAudioDevice(); // Removed to prevent conflicts when creating multiple MenuMode instances
	
	MenuMusic = LoadMusicStream((GameInstance::GetInstance()->g_WorkingDirectory + "/resources/music/Untitled.mp3").c_str());
	MenuMusic.looping = true;
	PlayMusicStream(MenuMusic);
	SetMusicVolume(MenuMusic, 0.02f);

	UIDispatcher = std::make_shared<EventDispatcher>();
	UIDispatcher->m_Name = "UIDispatcher Menu";

	BackgroundResource = GameInstance::GetInstance()->GetResource("BackgroundMenu");
	BackgroundResource->SetHeight(GetScreenHeight());
	BackgroundResource->SetWidth(GetScreenWidth());
	BackgroundResource->SetWidth(GetScreenWidth());
	Background = BackgroundResource->LoadTexture();

	TextureResource* ButtonResource = GameInstance::GetInstance()->GetResource("ButtonImage");
	SharedTexture2D SpriteButton = ButtonResource->LoadTexture();
	NPatchInfo ninePatchInfo1 = ButtonResource->nPatchInfo.value();

	WindowProperties CurrentProperties = GameInstance::GetInstance()->GetWindowProperties();
	int CenterX = CurrentProperties.m_ScreenWidth / 2;

#if DEBUG

	Rectangle SandboxRec = { CenterX - ButtonWidth / 2, 100, ButtonWidth, ButtonHeight };
	Sandbox = m_ObjectFactory->NewObject<Button>();
	Sandbox.TryLoad()->Construct(SandboxRec, "Sandbox", RED)
		.SetTexture(SpriteButton)
		.CenterText()
		.SetEventDispatcher(UIDispatcher)
		.SetEventPayload("Sandbox")
		.UseNPatchFeature(true)
		.UpdateTextColor(RED)
		.UseTexture(true)
		.SetNPatchInfo(ninePatchInfo1)
		.OnHover([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo->GetOfsettedNPatchInfo());
			})
		.OnHoverLeave([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo.value());
			});

#endif

	Rectangle StartGameRec = { CenterX - ButtonWidth / 2, 200, ButtonWidth, ButtonHeight };
	StartGame = m_ObjectFactory->NewObject<Button>();
	StartGame.TryLoad()->Construct(StartGameRec, "Start Game", RED)
		.CenterText()
		.UpdateTextColor(RED)
		.SetTexture(SpriteButton)
		.UseNPatchFeature(true)
		.UseTexture(true)
		.SetNPatchInfo(ninePatchInfo1)
		.OnHover([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo->GetOfsettedNPatchInfo());
			})
		.OnHoverLeave([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo.value());
			});

	Rectangle OptionRec = { CenterX - ButtonWidth / 2, 300, ButtonWidth, ButtonHeight };
	Option = m_ObjectFactory->NewObject<Button>();
	Option.TryLoad()->Construct(OptionRec, "Option", RED)
		.CenterText()
		.SetEventDispatcher(UIDispatcher)
		.SetEventPayload("Option")
		.UpdateTextColor(RED)
		.SetTexture(SpriteButton)
		.UseNPatchFeature(true)
		.UseTexture(true)
		.SetNPatchInfo(ninePatchInfo1)
		.OnHover([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo->GetOfsettedNPatchInfo());
			})
		.OnHoverLeave([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo.value());
			});

	Rectangle ExitRec = { CenterX - ButtonWidth / 2, 400, ButtonWidth, ButtonHeight };
	Exit = m_ObjectFactory->NewObject<Button>();
	Exit.TryLoad()->Construct(ExitRec, "Exit", RED).CenterText()
		.SetEventDispatcher(UIDispatcher)
		.SetEventPayload("Exit")
		.UpdateTextColor(RED)
		.SetTexture(SpriteButton)
		.UseNPatchFeature(true)
		.UseTexture(true)
		.SetNPatchInfo(ninePatchInfo1)
		.OnHover([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo->GetOfsettedNPatchInfo());
			})
		.OnHoverLeave([this, ButtonResource](Button* ButtonClass)
			{
				ButtonClass->SetNPatchInfo(ButtonResource->nPatchInfo.value());
			});

	SetUpEvents();
}

void MenuMode::SetName(std::string Name)
{
	m_Name = Name;
}

void MenuMode::SetUpEvents()
{
	// Windows Resize Event
	GameInstance::GetInstance()->AllPurposeDispatcher.AddListener("WindowsResize Menu", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event) -> void
		{
			std::cerr << "Called Event WindowResize" << std::endl;
			auto WindowResizeEvent = std::dynamic_pointer_cast<AllPurposeEvent>(Event);
			if (WindowResizeEvent == nullptr)
			{
				std::cerr << "Event Was nullptr" << std::endl;
				return;
			}

			if (WindowResizeEvent->Payload->GetStaticClass() != WindowResizeData::StaticClass())
			{
				std::cerr << "Not The Same Class" << std::endl;
				return;
			}



			std::shared_ptr<WindowResizeData> CurrentProperties = std::static_pointer_cast<WindowResizeData>(WindowResizeEvent->Payload);

			Height = CurrentProperties->height;
			Width = CurrentProperties->width;
			BackgroundResource->SetHeight(CurrentProperties->height);
			BackgroundResource->SetWidth(CurrentProperties->width);

#if DEBUG
			Sandbox.TryLoad()->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 100);
#endif
			StartGame.TryLoad()->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 200);
			Option.TryLoad()->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 300);
			Exit.TryLoad()->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 400);

		});

	// Exit Event
	UIDispatcher->AddListener("Exit Event", UIEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event) -> void
		{
			if (!Event || Event->GetStaticClass() != UIEvent::StaticClass())
			{
				return;
			}

			

			auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
			if (CastedEvent->Payload == "Exit")
			{
				CloseWindow();
			}
		});

	// OptionsEvent
	UIDispatcher->AddListener("Option Event", UIEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event) -> void
		{
			if (!Event || Event->GetStaticClass() != UIEvent::StaticClass())
			{
				return;
			}

			auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
			if (CastedEvent->Payload == "Option")
			{
				GameInstance::GetInstance()->g_ActiveStateMachine.ChangeState("Options");
			}
		});

	// Sandbox Event
	UIDispatcher->AddListener("Sandbox Event", UIEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event) -> void
		{
			if (!Event || Event->GetStaticClass() != UIEvent::StaticClass())
			{
				return;
			}

			auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
			if (CastedEvent->Payload == "Sandbox")
			{
				GameInstance::GetInstance()->g_ActiveStateMachine.ChangeState("Sandbox");
			}
		});
}

std::string MenuMode::GetName()
{
	return m_Name;
}
