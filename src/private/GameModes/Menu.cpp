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

MenuMode::MenuMode()
{
	SetName("Menu");

	UIDispatcher = std::make_shared<EventDispatcher>();
	UIDispatcher->Name = "UIDispatcher Menu";

	Image BackgroundImg = LoadImage((GameInstance::GetInstance()->WorkingDirectory + "\\resources\\imgs\\BackgroundMenu.jpg").c_str());
	Background = LoadTextureFromImage(BackgroundImg);
	UnloadImage(BackgroundImg);

	WindowProperties CurrentProperties = GameInstance::GetInstance()->GetWindowProperties();
	int CenterX = CurrentProperties.ScreenWidth / 2;

	Background.width = CurrentProperties.ScreenWidth;
	Background.height = CurrentProperties.ScreenHeight;

#if DEBUG

	Rectangle SandboxRec = { CenterX - ButtonWidth / 2, 100, ButtonWidth, 50 };
	Sandbox = std::make_shared<Button>();
	Sandbox->Construct(SandboxRec, "Sandbox", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Sandbox");

#endif
	
	Rectangle StartGameRec = { CenterX - ButtonWidth / 2, 200, ButtonWidth, 50 };
	StartGame = std::make_shared<Button>();
	StartGame->Construct(StartGameRec, "Start Game", RED).CenterText();

	Rectangle OptionRec = { CenterX - ButtonWidth / 2, 300, ButtonWidth, 50 };
	Option = std::make_shared<Button>();
	Option->Construct(OptionRec, "Option", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Option");

	Rectangle ExitRec = { CenterX - ButtonWidth / 2, 400, ButtonWidth, 50 };
	Exit = std::make_shared<Button>();
	Exit->Construct(ExitRec, "Exit", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Exit");

	SetUpEvents();
	
	std::cout << UIEvent::StaticClass().name() << std::endl;

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
				GameInstance::GetInstance()->ActiveStateMachine.ChangeState("Options");
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
				GameInstance::GetInstance()->ActiveStateMachine.ChangeState("Sandbox");
			}
		});
}

std::string MenuMode::GetName()
{
	return m_Name;
}
