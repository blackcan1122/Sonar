#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"
#include "Entities/BaseSubmarine.hpp"
#include "Base/Factory.hpp"
#include "Base/World.hpp"
#include "Events/SoundEvent.hpp"
#include "Events/DisplayResizeData.hpp"
#include "UI/GridLayoutManager.hpp"

SandboxGameMode::SandboxGameMode()
{
	SetName("Sandbox");

}

void SandboxGameMode::BeginPlay()
{
	GridLayoutManager* gridLayout = CreateGridLayout(2, 3);

	// Set up callbacks for display management
	gridLayout->SetDeleteDisplayCallback([this](SoftObjectPath<Display> display) {
		this->OnDeleteDisplay(display);
	});

	gridLayout->SetCreateDisplayCallback([this](const GridCell& cell, const DisplaySpawnInfo& spawnInfo) {
		this->OnCreateDisplay(cell, spawnInfo);
	});

	m_World = m_ObjectFactory->NewObject<World>();
	
	// Create MapDisplay and register with grid
	MapDisplay = m_ObjectFactory->NewObject<Map>(400, 400);
	if (auto map = MapDisplay.TryLoad())
	{
		gridLayout->RegisterDisplay(MapDisplay, GridCell{0, 1, 1, 1}); // Row 0, Col 1
		
		map->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
		{
			this->OnMapClickedEvent(Event);
		});

		map->OnResize.AddListener("SandboxGameMode Map Resize Listener", AllPurposeEvent::StaticClass(), gridLayout, &GridLayoutManager::OnDisplayResize);
		map->OnMove.AddListener("SandboxGameMode Map Move Listener", AllPurposeEvent::StaticClass(), gridLayout, &GridLayoutManager::OnDisplayMove);
	}

	PlayerOne = m_ObjectFactory->NewObject<Player>();
	PlayerOne.TryLoad()->SetEntityLocation(Vector2{ 0,0 });
	PlayerOne.TryLoad()->SetDisplayName("U-521");
	PlayerOne.TryLoad()->SetInitialSpeed(15);
	PlayerOne.TryLoad()->SetEntityRotationAndCourse(90);

	// Create WaterfallDisplay and register with grid
	WaterfallDisplay = m_ObjectFactory->NewObject<Waterfall>(360, 300, 10);
	if (auto waterfall = WaterfallDisplay.TryLoad())
	{
		gridLayout->RegisterDisplay(WaterfallDisplay, GridCell{0, 0, 1, 1}); // Row 0, Col 0
		waterfall->AssignPlayer(PlayerOne);
		waterfall->OnResize.AddListener("SandboxGameMode Waterfall Resize Listener", AllPurposeEvent::StaticClass(), gridLayout, &GridLayoutManager::OnDisplayResize);
		waterfall->OnMove.AddListener("SandboxGameMode Waterfall Move Listener", AllPurposeEvent::StaticClass(), gridLayout, &GridLayoutManager::OnDisplayMove);
	}
	m_AllDisplays.push_back(WaterfallDisplay);

	// Create WaterfallDisplay2 and register with grid
	WaterfallDisplay2 = m_ObjectFactory->NewObject<Waterfall>(360, 300, 60);
	if (auto waterfall2 = WaterfallDisplay2.TryLoad())
	{
		gridLayout->RegisterDisplay(WaterfallDisplay2, GridCell{1, 0, 1, 1}); // Row 1, Col 0
		waterfall2->AssignPlayer(PlayerOne);
		waterfall2->OnResize.AddListener("SandboxGameMode Waterfall2 Resize Listener", AllPurposeEvent::StaticClass(), gridLayout, &GridLayoutManager::OnDisplayResize);
		waterfall2->OnMove.AddListener("SandboxGameMode Waterfall2 Move Listener", AllPurposeEvent::StaticClass(), gridLayout, &GridLayoutManager::OnDisplayMove);
	}
	m_AllDisplays.push_back(WaterfallDisplay2);


	OtherSub = m_ObjectFactory->NewObject<BaseSubmarine>();
	OtherSub.TryLoad()->SetEntityLocation(Vector2{ 400,200 });
	OtherSub.TryLoad()->SetDisplayName("K-21");
	OtherSub.TryLoad()->SetInitialSpeed(15);
	OtherSub.TryLoad()->SetEntityRotationAndCourse(270);


	MapDisplay.TryLoad()->AddObjectToDraw(PlayerOne.TryLoad());
	MapDisplay.TryLoad()->AddObjectToDraw(OtherSub.TryLoad());

	// PlayerUI doesn't inherit from Display, so it's not managed by grid layout
	// It uses its own positioning system
	m_PlayerUI = m_ObjectFactory->NewObject<PlayerUI>(PlayerOne);
	m_PlayerUI.TryLoad()->SetPosition({static_cast<float>(gridLayout->GetCellWidth() * 2), 0});


}

void SandboxGameMode::Update()
{
		ClearBackground(BLACK);

		// Draw empty tile backgrounds first (beneath displays)
		if (m_GridLayoutManager)
		{
			m_GridLayoutManager->DrawEmptyTiles();
		}

		GameMode::Update();

		if (m_GridLayoutManager)
		{
			m_GridLayoutManager->DrawSnapPreviews();
		}

		// Draw grid controls (always visible)
		if (m_GridLayoutManager)
		{
			m_GridLayoutManager->DrawGridControls();
		}

		DrawFocusPlayer();


#if DEBUG
		DrawFPS(GameInstance::GetInstance()->GetWindowProperties().m_ScreenWidth - 100, 20);
#endif
		if (IsKeyPressed(KEY_K))
		{
			GameInstance::GetInstance()->g_ActiveStateMachine.ChangeState("Menu");
		}

		if (IsKeyPressed(KEY_L))
		{
			MapDisplay.TryLoad()->ResizeDisplay(600, 600);
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

void SandboxGameMode::OnDeleteDisplay(SoftObjectPath<Display> InDisplay)
{
	if (!InDisplay || !m_GridLayoutManager)
	{
		return;
	}
	
	// Unregister from grid first
	m_GridLayoutManager->UnregisterDisplay(InDisplay);
	
	for (auto it = m_AllDisplays.begin(); it != m_AllDisplays.end(); ++it)
	{
		if (*it == InDisplay)
		{
			m_AllDisplays.erase(it);
			break;
		}
	}

	InDisplay.TryLoad()->MarkForDestruction();
	this->DestroyObjectExplicitly<Display>(InDisplay);
}

void SandboxGameMode::OnCreateDisplay(const GridCell& cell, const DisplaySpawnInfo& spawnInfo)
{
	if (!m_GridLayoutManager)
	{
		return;
	}
	
	switch (spawnInfo.type)
	{
		case DisplayType::Map:
		{
			
			MapDisplay = m_ObjectFactory->NewObject<Map>(400, 400);
			if (auto map = MapDisplay.TryLoad())
			{
				m_GridLayoutManager->RegisterDisplay(MapDisplay, cell);
				
				// Re-add entities to the new map
				if (auto player = PlayerOne.TryLoad())
				{
					map->AddObjectToDraw(player);
				}
				if (auto otherSub = OtherSub.TryLoad())
				{
					map->AddObjectToDraw(otherSub);
				}
				
				map->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(),
					this, &SandboxGameMode::OnMapClickedEvent);
				map->OnResize.AddListener("SandboxGameMode Map Resize Listener", AllPurposeEvent::StaticClass(), m_GridLayoutManager.get(), &GridLayoutManager::OnDisplayResize);
				map->OnMove.AddListener("SandboxGameMode Map Move Listener", AllPurposeEvent::StaticClass(), m_GridLayoutManager.get(), &GridLayoutManager::OnDisplayMove);

				m_AllDisplays.push_back(MapDisplay);
			}
			break;
		}
		
		case DisplayType::Waterfall:
		{
			SoftObjectPath<Waterfall> newWaterfall = m_ObjectFactory->NewObject<Waterfall>(360, 300, spawnInfo.waterfallTimeframeSecs);
			
			if (auto waterfall = newWaterfall.TryLoad())
			{
				m_GridLayoutManager->RegisterDisplay(newWaterfall, cell);
				waterfall->AssignPlayer(PlayerOne);
				waterfall->OnResize.AddListener("SandboxGameMode Waterfall Resize Listener", AllPurposeEvent::StaticClass(), m_GridLayoutManager.get(), &GridLayoutManager::OnDisplayResize);
				waterfall->OnMove.AddListener("SandboxGameMode Waterfall Move Listener", AllPurposeEvent::StaticClass(), m_GridLayoutManager.get(), &GridLayoutManager::OnDisplayMove);
				m_AllDisplays.push_back(newWaterfall);
			}
			break;
		}
	}
}
