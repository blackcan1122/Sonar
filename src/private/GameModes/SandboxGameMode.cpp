#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"
#include "Entities/BaseSubmarine.hpp"
#include "Base/Factory.hpp"
#include "Base/World.hpp"

SandboxGameMode::SandboxGameMode()
{
	SetName("Sandbox");

}

void SandboxGameMode::BeginPlay()
{

	m_World = m_ObjectFactory->NewObject<World>();

	MapDisplay = m_ObjectFactory->NewObject<Map>(400, 400);
	MapDisplay.TryLoad()->SetPosition(Vector2{ 400,150 });

	MapDisplay.TryLoad()->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
		{
			this->OnMapClickedEvent(Event);
		});

	WaterfallDisplay = m_ObjectFactory->NewObject<Waterfall>(360, 300, 10);
	WaterfallDisplay.TryLoad()->SetPosition({ 0, 0 });

	WaterfallDisplay2 = m_ObjectFactory->NewObject<Waterfall>(360, 300, 60);
	WaterfallDisplay2.TryLoad()->SetPosition(Vector2{ 0,310 });

	

	PlayerOne = m_ObjectFactory->NewObject<Player>();
	PlayerOne.TryLoad()->SetEntityLocation(Vector2{ 20,30 });
	PlayerOne.TryLoad()->SetDisplayName("U-521");
	PlayerOne.TryLoad()->SetEntityRotation(0);
	PlayerOne.TryLoad()->ConvertAngleToVector();
	PlayerOne.TryLoad()->SetInitialSpeed(0);

	OtherSub = m_ObjectFactory->NewObject<BaseSubmarine>();
	OtherSub.TryLoad()->SetEntityLocation(Vector2{ 800,200 });
	OtherSub.TryLoad()->SetDisplayName("K-21");
	OtherSub.TryLoad()->SetInitialSpeed(15);


	MapDisplay.TryLoad()->AddObjectToDraw(PlayerOne.TryLoad());
	MapDisplay.TryLoad()->AddObjectToDraw(OtherSub.TryLoad());


	m_PlayerUI = m_ObjectFactory->NewObject<PlayerUI>(PlayerOne);
	m_PlayerUI.TryLoad()->SetPosition({500, 0});


}

void SandboxGameMode::Update()
{
		ClearBackground(BLACK);
		GameMode::Update();



		DrawFocusPlayer();


#if DEBUG
		DrawFPS(GameInstance::GetInstance()->GetWindowProperties().m_ScreenWidth - 100, 20);
#endif
		if (IsKeyPressed(KEY_K))
		{
			GameInstance::GetInstance()->g_ActiveStateMachine.ChangeState("Menu");
		}
}

void SandboxGameMode::SetName(std::string Name)
{
	m_Name = Name;
}



std::string SandboxGameMode::GetName()
{
	return m_Name;
}

void SandboxGameMode::OnMapClickedEvent(std::shared_ptr<IEvent> Event)
{
	if (!Event && Event->GetStaticClass() != AllPurposeEvent::StaticClass())
	{
		return;
	}

	std::shared_ptr<AllPurposeEvent> CastedEvent = std::dynamic_pointer_cast<AllPurposeEvent>(Event);
	if (CastedEvent->Payload->GetStaticClass() != MapClickEventData::StaticClass())
	{
		return;
	}

	std::shared_ptr<MapClickEventData> tempEventData = std::dynamic_pointer_cast<MapClickEventData>(CastedEvent->Payload);
	FocusedUnit = tempEventData->ClickedObject;
}

void SandboxGameMode::DrawFocusPlayer()
{
}
