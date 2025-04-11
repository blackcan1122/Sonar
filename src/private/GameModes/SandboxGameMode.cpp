#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"

SandboxGameMode::SandboxGameMode()
{
	SetName("Sandbox");

}

void SandboxGameMode::BeginPlay()
{
	WaterfallDisplay = m_ObjectFactory.NewObject<Waterfall>(360, 300, 10);
	WaterfallDisplay.TryLoad()->SetPosition({ 0, 0 });

	WaterfallDisplay2 = m_ObjectFactory.NewObject<Waterfall>(360, 300, 60);
	WaterfallDisplay2.TryLoad()->SetPosition(Vector2{ 0,310 });

	MapDisplay = m_ObjectFactory.NewObject<Map>(400, 400);
	MapDisplay.TryLoad()->SetPosition(Vector2{ 400,100 });

	MapDisplay.TryLoad()->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
		{
			this->OnMapClickedEvent(Event);
		});

	PlayerOne = m_ObjectFactory.NewObject<Player>();
	PlayerOne.TryLoad()->SetEntityLocation(Vector2{ 0,0 });
	PlayerOne.TryLoad()->SetDisplayName("U521");
	PlayerOne.TryLoad()->SetEntityRotation(0);
	PlayerOne.TryLoad()->ConvertAngleToVector();


	PlayerTwo = m_ObjectFactory.NewObject<Player>();
	PlayerTwo.TryLoad()->SetEntityLocation(Vector2{ 800,200 });
	PlayerTwo.TryLoad()->SetDisplayName("K-21");


	MapDisplay.TryLoad()->AddObjectToDraw(PlayerTwo.TryLoad());
	MapDisplay.TryLoad()->AddObjectToDraw(PlayerOne.TryLoad());

	m_PlayerUI = m_ObjectFactory.NewObject<PlayerUI>(PlayerOne);
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
		if (IsKeyPressed(KEY_R))
		{

			if (auto tempPlayer = FocusedUnit.Cast<Player>().TryLoad())
			{
				tempPlayer->SetEntityRotation(tempPlayer->GetEntityRotation() + 5);
				tempPlayer->ConvertAngleToVector();
			}
		}
		if (IsKeyPressed(KEY_N))
		{
			WaterfallDisplay = m_ObjectFactory.NewObject<Waterfall>(360, 360, 30);
			WaterfallDisplay.TryLoad()->SetPosition(Vector2{0,0});
		}


		if (IsKeyDown(KEY_W))
		{

			if (auto tempPlayer = FocusedUnit.Cast<Player>().TryLoad())
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

	m_PlayerUI.TryLoad()->AssignedPlayer = tempEventData->ClickedObject.Cast<Player>();
	FocusedUnit = tempEventData->ClickedObject;
}

void SandboxGameMode::DrawFocusPlayer()
{
	std::string Name;
	std::string Location;
	if (auto FocusedUnitObj = FocusedUnit.TryLoad())
	{
		// Need to have a Display name as a member
		Name = "Name: " + FocusedUnitObj->GetDisplayName();
		Location = "X: " + std::to_string(FocusedUnitObj->GetEntityLocation().x) + ", Y: " + std::to_string(FocusedUnitObj->GetEntityLocation().y);
	}
	else
	{
		Name = "Name: None";
		Location = "X:?, Y: ?";
	}

	DrawText(Name.c_str(), 600, 600, 24, GREEN);
	DrawText(Location.c_str(), 600, 620, 24, GREEN);
}
