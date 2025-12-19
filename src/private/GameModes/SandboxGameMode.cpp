#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"
#include "Entities/BaseSubmarine.hpp"
#include "Base/Factory.hpp"
#include "Base/World.hpp"
#include "Events/SoundEvent.hpp"
#include "UI/GridLayoutManager.hpp"

SandboxGameMode::SandboxGameMode()
{
	SetName("Sandbox");

}

void SandboxGameMode::BeginPlay()
{
	// Create a 2x3 grid layout (2 rows, 3 columns)
	GridLayoutManager* gridLayout = CreateGridLayout(2, 3);
	
	// Set up callbacks for display management
	gridLayout->SetDeleteDisplayCallback([this](Display* display) {
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
		map->SetGridLayoutManager(gridLayout);
		gridLayout->RegisterDisplay(map.get(), GridCell{0, 1, 1, 1}); // Row 0, Col 1
		
		map->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
		{
			this->OnMapClickedEvent(Event);
		});
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
		waterfall->SetGridLayoutManager(gridLayout);
		gridLayout->RegisterDisplay(waterfall.get(), GridCell{0, 0, 1, 1}); // Row 0, Col 0
		waterfall->AssignPlayer(PlayerOne);
	}
	m_WaterfallDisplays.push_back(WaterfallDisplay);

	// Create WaterfallDisplay2 and register with grid
	WaterfallDisplay2 = m_ObjectFactory->NewObject<Waterfall>(360, 300, 60);
	if (auto waterfall2 = WaterfallDisplay2.TryLoad())
	{
		waterfall2->SetGridLayoutManager(gridLayout);
		gridLayout->RegisterDisplay(waterfall2.get(), GridCell{1, 0, 1, 1}); // Row 1, Col 0
		waterfall2->AssignPlayer(PlayerOne);
	}
	m_WaterfallDisplays.push_back(WaterfallDisplay2);


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

		// Draw snap previews on top of all displays
		if (m_GridLayoutManager)
		{
			m_GridLayoutManager->DrawSnapPreviews();
		}

		// Draw grid controls (always visible)
		if (m_GridLayoutManager)
		{
			m_GridLayoutManager->DrawGridControls();
		}

		// Draw debug grid overlay in debug mode
#if DEBUG
		// if (m_GridLayoutManager)
		// {
		// 	m_GridLayoutManager->DrawDebugGrid();
		// }
#endif

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
		
		// Grid layout controls
		if (m_GridLayoutManager)
		{
			// Add row with Ctrl+Plus
			if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_EQUAL))
			{
				m_GridLayoutManager->AddRow();
			}
			// Remove row with Ctrl+Minus
			if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_MINUS))
			{
				m_GridLayoutManager->RemoveRow();
			}
			// Add column with Shift+Plus
			if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_EQUAL))
			{
				m_GridLayoutManager->AddColumn();
			}
			// Remove column with Shift+Minus
			if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_MINUS))
			{
				m_GridLayoutManager->RemoveColumn();
			}
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

void SandboxGameMode::OnDeleteDisplay(Display* display)
{
	if (!display || !m_GridLayoutManager)
	{
		return;
	}
	
	// Unregister from grid first
	m_GridLayoutManager->UnregisterDisplay(display);
	
	// Find and remove from our tracking list
	for (auto it = m_WaterfallDisplays.begin(); it != m_WaterfallDisplays.end(); ++it)
	{
		if (auto waterfall = it->TryLoad())
		{
			if (waterfall.get() == display)
			{
				// Mark for destruction - the Factory deleter will handle cleanup
				waterfall->MarkForDestruction();
				m_WaterfallDisplays.erase(it);
				return;
			}
		}
	}
	
	// Check if it's the map display
	if (auto map = MapDisplay.TryLoad())
	{
		if (map.get() == display)
		{
			map->MarkForDestruction();
			MapDisplay = SoftObjectPath<Map>(); // Clear the reference
			return;
		}
	}
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
				map->SetGridLayoutManager(m_GridLayoutManager.get());
				m_GridLayoutManager->RegisterDisplay(map.get(), cell);
				
				// Re-add entities to the new map
				if (auto player = PlayerOne.TryLoad())
				{
					map->AddObjectToDraw(player);
				}
				if (auto otherSub = OtherSub.TryLoad())
				{
					map->AddObjectToDraw(otherSub);
				}
				
				map->MapEventDispatcher->AddListener("Map Events", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
				{
					this->OnMapClickedEvent(Event);
				});
			}
			break;
		}
		
		case DisplayType::Waterfall:
		{
			SoftObjectPath<Waterfall> newWaterfall = m_ObjectFactory->NewObject<Waterfall>(360, 300, spawnInfo.waterfallTimeframeSecs);
			
			if (auto waterfall = newWaterfall.TryLoad())
			{
				waterfall->SetGridLayoutManager(m_GridLayoutManager.get());
				m_GridLayoutManager->RegisterDisplay(waterfall.get(), cell);
				waterfall->AssignPlayer(PlayerOne);
				m_WaterfallDisplays.push_back(newWaterfall);
			}
			break;
		}
	}
}
