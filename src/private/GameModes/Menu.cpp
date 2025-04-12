#include "GameModes/Menu.hpp"
#include "Base/GameInstance.h"
#include "Base/StateMachine.h"

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

	UIDispatcher = std::make_shared<EventDispatcher>();
	UIDispatcher->m_Name = "UIDispatcher Menu";

	Image BackgroundImg = LoadImage((GameInstance::GetInstance()->g_WorkingDirectory + "\\resources\\imgs\\BackgroundMenu.jpg").c_str());
	Background = LoadTextureFromImage(BackgroundImg);
	UnloadImage(BackgroundImg);

	TextureResource* ButtonResource = GameInstance::GetInstance()->GetResource("ButtonImage");
	Texture2D nPatchTexture = LoadTextureFromImage(ButtonResource->ImageTexture);
	NPatchInfo ninePatchInfo1 = ButtonResource->nPatchInfo.value();

	WindowProperties CurrentProperties = GameInstance::GetInstance()->GetWindowProperties();
	int CenterX = CurrentProperties.m_ScreenWidth / 2;

	Background.width = CurrentProperties.m_ScreenWidth;
	Background.height = CurrentProperties.m_ScreenHeight;

#if DEBUG

	Rectangle SandboxRec = { CenterX - ButtonWidth / 2, 100, ButtonWidth, ButtonHeight };
	Sandbox = std::make_shared<Button>();
	Sandbox->Construct(SandboxRec, "Sandbox", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Sandbox");

	
	Sandbox->SetTexture(nPatchTexture)
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
	StartGame = std::make_shared<Button>();
	StartGame->Construct(StartGameRec, "Start Game", RED).CenterText()
		.UpdateTextColor(RED)
		.SetTexture(nPatchTexture)
		.UseNPatchFeature(true)
		.UseTexture(true)
		.SetNPatchInfo(ninePatchInfo1)
		.OnHover([this](Button* ButtonClass)
			{
				NPatchInfo ninePatchInfo1 = { Rectangle { 128.0f, 0.0f, 128.0f, 128.0f }, 32, 32, 32, 32, NPATCH_NINE_PATCH };
				ButtonClass->SetNPatchInfo(ninePatchInfo1);
			})
		.OnHoverLeave([this](Button* ButtonClass)
			{
				NPatchInfo ninePatchInfo1 = { Rectangle { 0.0f, 0.0f, 128.0f, 128.0f }, 32, 32, 32, 32, NPATCH_NINE_PATCH };
				ButtonClass->SetNPatchInfo(ninePatchInfo1);
			});

	Rectangle OptionRec = { CenterX - ButtonWidth / 2, 300, ButtonWidth, ButtonHeight };
	Option = std::make_shared<Button>();
	Option->Construct(OptionRec, "Option", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Option")
		.UpdateTextColor(RED)
		.SetTexture(nPatchTexture)
		.UseNPatchFeature(true)
		.UseTexture(true)
		.SetNPatchInfo(ninePatchInfo1)
		.OnHover([this](Button* ButtonClass)
			{
				NPatchInfo ninePatchInfo1 = { Rectangle { 128.0f, 0.0f, 128.0f, 128.0f }, 32, 32, 32, 32, NPATCH_NINE_PATCH };
				ButtonClass->SetNPatchInfo(ninePatchInfo1);
			})
		.OnHoverLeave([this](Button* ButtonClass)
			{
				NPatchInfo ninePatchInfo1 = { Rectangle { 0.0f, 0.0f, 128.0f, 128.0f }, 32, 32, 32, 32, NPATCH_NINE_PATCH };
				ButtonClass->SetNPatchInfo(ninePatchInfo1);
			});

	Rectangle ExitRec = { CenterX - ButtonWidth / 2, 400, ButtonWidth, ButtonHeight };
	Exit = std::make_shared<Button>();
	Exit->Construct(ExitRec, "Exit", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Exit")
		.UpdateTextColor(RED)
		.SetTexture(nPatchTexture)
		.UseNPatchFeature(true)
		.UseTexture(true)
		.SetNPatchInfo(ninePatchInfo1)
		.OnHover([this](Button* ButtonClass)
			{
				NPatchInfo ninePatchInfo1 = { Rectangle { 128.0f, 0.0f, 128.0f, 128.0f }, 32, 32, 32, 32, NPATCH_NINE_PATCH };
				ButtonClass->SetNPatchInfo(ninePatchInfo1);
			})
		.OnHoverLeave([this](Button* ButtonClass)
			{
				NPatchInfo ninePatchInfo1 = { Rectangle { 0.0f, 0.0f, 128.0f, 128.0f }, 32, 32, 32, 32, NPATCH_NINE_PATCH };
				ButtonClass->SetNPatchInfo(ninePatchInfo1);
			});

	SetUpEvents();
}

MenuMode::~MenuMode()
{
	GameInstance::GetInstance()->AllPurposeDispatcher.RemoveListener("WindowsResize Menu", AllPurposeEvent::StaticClass());
	UnloadTexture(Background);
}

void MenuMode::Update()
{
	ClearBackground(GREEN);
	DrawTexture(Background,0,0,WHITE);

	float DeltaTime = GetFrameTime();
	
#if DEBUG
	Sandbox->Tick(DeltaTime);
#endif

	StartGame->Tick(DeltaTime);
	Option->Tick(DeltaTime);
	Exit->Tick(DeltaTime);


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

			Background.height = CurrentProperties->height;
			Background.width = CurrentProperties->width;
#if DEBUG
			Sandbox->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 100);
#endif
			StartGame->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 200);
			Option->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 300);
			Exit->UpdateButtonPosition((CurrentProperties->width / 2) - (ButtonWidth / 2), 400);

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
