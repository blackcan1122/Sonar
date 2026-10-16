#include "GameModes/SandboxGameMode.hpp"
#include "Entities/BaseSubmarine.hpp"
#include "Base/Factory.hpp"
#include "Base/World.hpp"
#include "Events/DisplayResizeData.hpp"
#include "UI/GridLayoutManager.hpp"

std::string Test{ "DuFicker" };

SandboxGameMode::SandboxGameMode()
{
	SetName("Sandbox");

}

void SandboxGameMode::BeginPlay()
{
	m_LayoutManager = NewObject<GridLayoutManager>(2, 2, GetRuntimeContext().GetScreenWidth(), GetRuntimeContext().GetScreenHeight());

	auto LayoutManagerObj = m_LayoutManager.TryLoad();
	if (!LayoutManagerObj)
	{
		return;
	}

	// Set up callbacks for display management
	LayoutManagerObj->SetDeleteDisplayCallback([this](SoftObjectPath<Display> display) {
		this->OnDeleteDisplay(display);
	});

	LayoutManagerObj->SetCreateDisplayCallback([this](const GridCell& Cell, const LayoutSpawnRequest& SpawnRequest) {
		this->OnCreateDisplay(Cell, SpawnRequest);
	});

	LayoutManagerObj->SetSpawnOptions({
		{ "map", "Map", 0 },
		{ "waterfall", "Waterfall 10 Seconds", 10 },
		{ "waterfall", "Waterfall 30 Seconds", 30 },
		{ "waterfall", "Waterfall 60 Seconds", 60 },
		{ "waterfall", "Waterfall 120 Seconds", 120 },
		{ "waterfall", "Waterfall 300 Seconds", 300 }
	});

	m_World =  NewObject<World>();
	m_SignalSystem = NewObject<SignalSystem>();
	
	MapDisplay = NewObject<Map>(400, 400);
	if (auto map = MapDisplay.TryLoad())
	{
		LayoutManagerObj->RegisterDisplay(MapDisplay, GridCell{0, 1, 1, 1}); // Row 0, Col 1
		
		map->MapEventDispatcher->AddListener("SandboxGameMode Map Click Listener", AllPurposeEvent::StaticClass(), this, &SandboxGameMode::OnMapClickedEvent);
	}

	PlayerOne =  NewObject<SubmarinePlayer>();
	PlayerOne.TryLoad()->SetEntityLocation(Vector2{ 0,0 });
	PlayerOne.TryLoad()->SetDisplayName("U-521");
	PlayerOne.TryLoad()->SetInitialSpeed(0);
	PlayerOne.TryLoad()->SetEntityRotationAndCourse(90);

	// Create WaterfallDisplay and register with grid
	WaterfallDisplay =  NewObject<Waterfall>(360, 300, 10);
	if (auto waterfall = WaterfallDisplay.TryLoad())
	{
		LayoutManagerObj->RegisterDisplay(WaterfallDisplay, GridCell{0, 0, 1, 1}); // Row 0, Col 0
		waterfall->AssignPlayer(PlayerOne);
		waterfall->AssignSignalSystem(m_SignalSystem);

	}
	m_AllDisplays.push_back(WaterfallDisplay);

	// Create WaterfallDisplay2 and register with grid
	WaterfallDisplay2 =  NewObject<Waterfall>(360, 300, 60);
	if (auto waterfall2 = WaterfallDisplay2.TryLoad())
	{
		LayoutManagerObj->RegisterDisplay(WaterfallDisplay2, GridCell{1, 0, 1, 1}); // Row 1, Col 0
		waterfall2->AssignPlayer(PlayerOne);
		waterfall2->AssignSignalSystem(m_SignalSystem);
	}
	m_AllDisplays.push_back(WaterfallDisplay2);


	OtherSub =  NewObject<BaseSubmarine>();
	OtherSub.TryLoad()->SetEntityLocation(Vector2{ 400,200 });
	OtherSub.TryLoad()->SetDisplayName("K-21");
	OtherSub.TryLoad()->SetInitialSpeed(15);
	OtherSub.TryLoad()->SetEntityRotationAndCourse(270);


	MapDisplay.TryLoad()->AddObjectToDraw(PlayerOne.TryLoad());
	MapDisplay.TryLoad()->AddObjectToDraw(OtherSub.TryLoad());

	// PlayerUI doesn't inherit from Display, so it's not managed by grid layout
	// It uses its own positioning system
	m_PlayerUI =  NewObject<PlayerUI>(PlayerOne);
	m_PlayerUI.TryLoad()->SetPosition({static_cast<float>(LayoutManagerObj->GetCellWidth() * 2), 0});


}

void SandboxGameMode::Update()
{
		ClearBackground(BLACK);
		GameMode::Update();


		if (auto prop = Entity::StaticClass()->FindProperty("m_Position"))
		{
			Vector2 Test = prop->GetValue<Vector2>(PlayerOne.TryLoad().get());
		}

		Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), Vector2{ 500, 500 });
		//Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), int(20)); // Wrong type test
		//Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), true); // Wrong type test

#if DEBUG
		DrawFPS(GetRuntimeContext().GetScreenWidth() - 100, 20);
#endif
		if (IsKeyPressed(KEY_K))
		{
			GetRuntimeContext().RequestGameModeChange("Menu");
		}

		if (IsKeyPressed(KEY_T))
		{
			for (auto prop : SubmarinePlayer::StaticClass()->GetAllProperties())
			{
				std::cout << prop->PropertyName << std::endl;
			}
			SubmarinePlayer::StaticClass()->SetPropertyValue("m_DisplayName", PlayerOne.TryLoad().get(), std::string("FIIICKER"));
		}

		if (IsKeyPressed(KEY_Y))
		{
			SubmarinePlayer::StaticClass()->SetPropertyValue("m_AccelerationRate", PlayerOne.TryLoad().get(), 200.f);
		}

		if (IsKeyPressed(KEY_L))
		{
			//SubmarinePlayer* PlayerOnePtr = nullptr;
			//PlayerOnePtr->GetDisplayName();
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
	if (!Event || Event->GetStaticClass() != AllPurposeEvent::StaticClass())
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

void SandboxGameMode::OnDeleteDisplay(SoftObjectPath<Display> InDisplay)
{
	if (!InDisplay)
	{
		return;
	}
	
	for (auto it = m_AllDisplays.begin(); it != m_AllDisplays.end(); ++it)
	{
		if (*it == InDisplay)
		{
			m_AllDisplays.erase(it);
			break;
		}
	}

	InDisplay.TryLoad()->MarkForDestruction();
}

void SandboxGameMode::OnCreateDisplay(const GridCell& Cell, const LayoutSpawnRequest& SpawnRequest)
{
	auto LayoutManagerObj = m_LayoutManager.TryLoad();
	if (!LayoutManagerObj)
	{
		return;
	}

	if (SpawnRequest.SpawnId == "map")
	{
		SoftObjectPath<Map> MapDisplayToCreate = NewObject<Map>(400, 400);
		if (auto MapObj = MapDisplayToCreate.TryLoad())
		{
			if (LayoutManagerObj->RegisterDisplay(MapDisplayToCreate, Cell))
			{
				if (auto playerone = PlayerOne.TryLoad())
				{
					MapObj->AddObjectToDraw(playerone);
				}
				if (auto otherSub = OtherSub.TryLoad())
				{
					MapObj->AddObjectToDraw(otherSub);
				}

				MapObj->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(),
					this, &SandboxGameMode::OnMapClickedEvent);

				m_AllDisplays.push_back(MapDisplayToCreate);
			}
			else
			{
				MapObj->MarkForDestruction();
			}
		}
		return;
	}

	if (SpawnRequest.SpawnId == "waterfall")
	{
		SoftObjectPath<Waterfall> WaterfallDisplayToCreate = NewObject<Waterfall>(360, 300, SpawnRequest.IntegerPayload);
		if (auto waterfall = WaterfallDisplayToCreate.TryLoad())
		{
			if (LayoutManagerObj->RegisterDisplay(WaterfallDisplayToCreate, Cell))
			{
				waterfall->AssignPlayer(PlayerOne);
				waterfall->AssignSignalSystem(m_SignalSystem);
				m_AllDisplays.push_back(WaterfallDisplayToCreate);
			}
			else
			{
				waterfall->MarkForDestruction();
			}
		}
	}
}
