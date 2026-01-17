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
	auto Window = GameInstance::GetInstance()->GetWindowProperties();
	SoftObjectPath<GridLayoutManager> LayoutManager = NewObject<GridLayoutManager>(2, 2, Window.m_ScreenWidth, Window.m_ScreenHeight);

	auto LayoutManagerObj = LayoutManager.TryLoad();
	// Set up callbacks for display management
	LayoutManagerObj->SetDeleteDisplayCallback([this](SoftObjectPath<Display> display) {
		this->OnDeleteDisplay(display);
	});

	LayoutManagerObj->SetCreateDisplayCallback([this](const SoftObjectPath<Display>InDisplay, const DisplaySpawnInfo InDisplaySpawnInfo) {
		this->OnCreateDisplay(InDisplay, InDisplaySpawnInfo);
	});

	m_World =  NewObject<World>();
	
	MapDisplay = NewObject<Map>(400, 400);
	if (auto map = MapDisplay.TryLoad())
	{
		LayoutManagerObj->RegisterDisplay(MapDisplay, GridCell{0, 1, 1, 1}); // Row 0, Col 1
		
		map->MapEventDispatcher->AddListener("SandboxGameMode Map Click Listener", AllPurposeEvent::StaticClass(), this, &SandboxGameMode::OnMapClickedEvent);
		map->OnResize.AddListener("SandboxGameMode Map Resize Listener", AllPurposeEvent::StaticClass(), LayoutManager, &GridLayoutManager::OnDisplayResize);
		map->OnMove.AddListener("SandboxGameMode Map Move Listener", AllPurposeEvent::StaticClass(), LayoutManager, &GridLayoutManager::OnDisplayMove);
	}

	PlayerOne =  NewObject<Player>();
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
		waterfall->OnResize.AddListener("SandboxGameMode Waterfall Resize Listener", AllPurposeEvent::StaticClass(), LayoutManager, &GridLayoutManager::OnDisplayResize);
		waterfall->OnMove.AddListener("SandboxGameMode Waterfall Move Listener", AllPurposeEvent::StaticClass(), LayoutManager, &GridLayoutManager::OnDisplayMove);
	}
	m_AllDisplays.push_back(WaterfallDisplay);

	// Create WaterfallDisplay2 and register with grid
	WaterfallDisplay2 =  NewObject<Waterfall>(360, 300, 60);
	if (auto waterfall2 = WaterfallDisplay2.TryLoad())
	{
		LayoutManagerObj->RegisterDisplay(WaterfallDisplay2, GridCell{1, 0, 1, 1}); // Row 1, Col 0
		waterfall2->AssignPlayer(PlayerOne);
		waterfall2->OnResize.AddListener("SandboxGameMode Waterfall2 Resize Listener", AllPurposeEvent::StaticClass(), LayoutManager, &GridLayoutManager::OnDisplayResize);
		waterfall2->OnMove.AddListener("SandboxGameMode Waterfall2 Move Listener", AllPurposeEvent::StaticClass(), LayoutManager, &GridLayoutManager::OnDisplayMove);
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

		if (auto prop = Entity::StaticClass()->FindProperty("m_Position"))
		{
			Vector2 Test = prop->GetValue<Vector2>(PlayerOne.TryLoad().get());
			LOG_INFO(l_GAME_INSTANCE, TEXT("Entity Position X: {} Y: {}", Test.x, Test.y));
		}

		Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), Vector2{ 500, 500 });
		Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), int(20)); // Wrong type test
		Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), true); // Wrong type test

		auto GLM = GetObjects<GridLayoutManager>();
		if (GLM.empty())
		{
			return;
		}

		auto GLMOBJ = GLM[0].TryLoad();

		if (!GLMOBJ)
		{
			return;
		}

		if (GLMOBJ)
		{
			GLMOBJ->DrawEmptyTiles();
		}

		GameMode::Update();

		if (GLMOBJ)
		{
			GLMOBJ->DrawSnapPreviews();
		}

		// Draw grid controls (always visible)
		if (GLMOBJ)
		{
			GLMOBJ->DrawGridControls();
		}

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

void SandboxGameMode::OnCreateDisplay(const SoftObjectPath<Display> InDisplay, const DisplaySpawnInfo inDisplayType)
{
	switch (inDisplayType.type)
	{
		case DisplayType::Map:
		{
			if (auto MapObj = InDisplay.Cast<Map>().TryLoad())
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

				m_AllDisplays.push_back(InDisplay);
			}
			break;
		}
		
		case DisplayType::Waterfall:
		{	
			if (auto waterfall = InDisplay.Cast<Waterfall>().TryLoad())
			{
				waterfall->AssignPlayer(PlayerOne);
				m_AllDisplays.push_back(InDisplay);
			}
			break;
		}
	}
}
