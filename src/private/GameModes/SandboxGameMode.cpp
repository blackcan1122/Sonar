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

	WaterfallDisplay = m_ObjectFactory->NewObject<Waterfall>(360, 300, 10);
	WaterfallDisplay.TryLoad()->SetPosition({ 0, 0 });

	WaterfallDisplay2 = m_ObjectFactory->NewObject<Waterfall>(360, 300, 60);
	WaterfallDisplay2.TryLoad()->SetPosition(Vector2{ 0,310 });

	MapDisplay = m_ObjectFactory->NewObject<Map>(400, 400);
	MapDisplay.TryLoad()->SetPosition(Vector2{ 400,100 });

	MapDisplay.TryLoad()->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
		{
			this->OnMapClickedEvent(Event);
		});

	PlayerOne = m_ObjectFactory->NewObject<Player>();
	PlayerOne.TryLoad()->SetEntityLocation(Vector2{ 0,0 });
	PlayerOne.TryLoad()->SetDisplayName("U-521");
	PlayerOne.TryLoad()->SetEntityRotation(0);
	PlayerOne.TryLoad()->ConvertAngleToVector();
	PlayerOne.TryLoad()->SetInitialSpeed(5);

	OtherSub = m_ObjectFactory->NewObject<BaseSubmarine>();
	OtherSub.TryLoad()->SetEntityLocation(Vector2{ 800,200 });
	OtherSub.TryLoad()->SetDisplayName("K-21");


	MapDisplay.TryLoad()->AddObjectToDraw(PlayerOne.TryLoad());
	MapDisplay.TryLoad()->AddObjectToDraw(OtherSub.TryLoad());


	m_PlayerUI = m_ObjectFactory->NewObject<PlayerUI>(PlayerOne);
	m_PlayerUI.TryLoad()->SetPosition({500, 0});


}

SandboxGameMode::~SandboxGameMode()
{
}

void SandboxGameMode::Update()
{
		ClearBackground(RED);
		GameMode::Update();

		if (IsKeyPressed(KEY_D))
		{
			PlayerOne.TryLoad()->MarkForDestruction();
		}
		if (IsKeyPressed(KEY_N))
		{
			WaterfallDisplay = m_ObjectFactory->NewObject<Waterfall>(360, 360, 30);
			WaterfallDisplay.TryLoad()->SetPosition(Vector2{ 0,0 });
		}

		if (IsKeyPressed(KEY_R))
		{

			if (auto tempPlayer = FocusedUnit.Cast<Player>().TryLoad())
			{
				tempPlayer->SetCourse(180.f);
			}
		}

		if (IsKeyDown(KEY_W))
		{

			if (auto tempPlayer = FocusedUnit.Cast<BaseSubmarine>().TryLoad())
			{
				tempPlayer->SetSpeed(10);
			}
		}


		DrawFocusPlayer();


#if DEBUG
		DrawFPS(GameInstance::GetInstance()->GetWindowProperties().m_ScreenWidth - 100, 20);
#endif

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
