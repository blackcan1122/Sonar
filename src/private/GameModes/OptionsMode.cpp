#include "GameModes/OptionsMode.hpp"
#include "Base/GameInstance.h"
#include "Base/StateMachine.h"

// Events
#include "Events/AllPurposeEvent.h"
#include "Events/UIEvent.h"

// EventData
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

#define ButtonWidth 200
#define ButtonHeight 50
#define Padding 20

OptionsMode::OptionsMode()
{
	SetName("Options");
}

OptionsMode::~OptionsMode()
{
	GameInstance::GetInstance()->AllPurposeDispatcher.RemoveListener("WindowsResize Option", AllPurposeEvent::StaticClass());
}

void OptionsMode::Update()
{
	ClearBackground(PURPLE);
	DrawTexture(Background, 0, 0, WHITE);

	float DeltaTime = GetFrameTime();


	Back->Tick(DeltaTime);
	Apply->Tick(DeltaTime);
}

void OptionsMode::BeginPlay()
{
	UIDispatcher = std::make_shared<EventDispatcher>();
	UIDispatcher->m_Name = "UIDispatcher Menu";

	TextureResource* BackgroundResource = GameInstance::GetInstance()->GetResource("BackgroundMenu");
	Background = BackgroundResource->LoadTexture();

	WindowProperties CurrentProperties = GameInstance::GetInstance()->GetWindowProperties();
	int CenterX = CurrentProperties.m_ScreenWidth / 2;

	Rectangle ApplyRec = { GetScreenWidth() - (ButtonWidth + Padding), GetScreenHeight() - (ButtonHeight + Padding), ButtonWidth, ButtonHeight };
	Apply = std::make_shared<Button>();
	Apply->Construct(ApplyRec, "Apply", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Apply");

	Rectangle BackRec = { Padding, GetScreenHeight() - (ButtonHeight + Padding), ButtonWidth, ButtonHeight };
	Back = std::make_shared<Button>();
	Back->Construct(BackRec, "Back", RED).CenterText().SetEventDispatcher(UIDispatcher).SetEventPayload("Back");

	SetUpEvents();
}

void OptionsMode::SetName(std::string Name)
{
	m_Name = Name;
}

void OptionsMode::SetUpEvents()
{
	// Windows Resize Event
	GameInstance::GetInstance()->AllPurposeDispatcher.AddListener("WindowsResize Option", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event) -> void
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

			Back->UpdateButtonPosition(Padding, Height - (ButtonHeight + Padding));
			Apply->UpdateButtonPosition(Width - (ButtonWidth + Padding), Height - (ButtonHeight + Padding));

		});

	// Exit Event
	UIDispatcher->AddListener("Apply", UIEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event) -> void
		{
			if (!Event || Event->GetStaticClass() != UIEvent::StaticClass())
			{
				return;
			}

			auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
			if (CastedEvent->Payload == "Apply")
			{
				// TODO
			}
		});

	UIDispatcher->AddListener("Back", UIEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event) -> void
		{
			if (!Event || Event->GetStaticClass() != UIEvent::StaticClass())
			{
				return;
			}

			auto CastedEvent = std::dynamic_pointer_cast<UIEvent>(Event);
			if (CastedEvent->Payload == "Back")
			{
				GameInstance::GetInstance()->g_ActiveStateMachine.ChangeState("Menu");
			}
		});
}

std::string OptionsMode::GetName()
{
	return m_Name;
}
