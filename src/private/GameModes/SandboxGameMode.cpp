#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"
#include "Entities/BaseSubmarine.hpp"
#include "Base/Factory.hpp"
#include "Base/World.hpp"
#include "Events/SoundEvent.hpp"
#include "Events/DisplayResizeData.hpp"
#include "UI/GridLayoutManager.hpp"

std::string Test{ "DuFicker" };

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

	}
	m_AllDisplays.push_back(WaterfallDisplay);

	// Create WaterfallDisplay2 and register with grid
	WaterfallDisplay2 =  NewObject<Waterfall>(360, 300, 60);
	if (auto waterfall2 = WaterfallDisplay2.TryLoad())
	{
		LayoutManagerObj->RegisterDisplay(WaterfallDisplay2, GridCell{1, 0, 1, 1}); // Row 1, Col 0
		waterfall2->AssignPlayer(PlayerOne);
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
		Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), int(20)); // Wrong type test
		Entity::StaticClass()->SetPropertyValue("m_Position", PlayerOne.TryLoad().get(), true); // Wrong type test

#if DEBUG
		DrawFPS(GameInstance::GetInstance()->GetWindowProperties().m_ScreenWidth - 100, 20);
#endif
		if (IsKeyPressed(KEY_K))
		{
			GameInstance::GetInstance()->g_ActiveStateMachine.ChangeState("Menu");
		}

		if (IsKeyPressed(KEY_T))
		{
			{
				auto player = PlayerOne.TryLoad().get();

				// Direct address
				void* direct = player->GetDisplayNamePtr();

				// Reflected address
				auto prop = IObject::StaticClass()->FindProperty("m_DisplayName");
				void* reflected = prop->GetValuePtr(player);

				std::cout << "direct:    " << direct << std::endl;
				std::cout << "reflected: " << reflected << std::endl;
				std::cout << "match: " << (direct == reflected) << std::endl;

				Player* p = PlayerOne.TryLoad().get();
				IObject* i = p;  // typed cast — compiler adjusts

				std::cout << "Player* : " << (void*)p << std::endl;
				std::cout << "IObject*: " << (void*)i << std::endl;

				// Crack open the member pointer offset directly
				auto* sProp = static_cast<SProperty<IObject, std::string>*>(prop);

				union {
					std::string IObject::* mp;
					ptrdiff_t offset;
				} u;
				u.mp = &IObject::m_DisplayName;
				std::cout << "m_DisplayName member pointer offset: " << u.offset << std::endl;

				u.mp = &IObject::m_Name;
				std::cout << "m_Name member pointer offset: " << u.offset << std::endl;

				IObject stackObj;
				std::cout << "sizeof IObject: " << sizeof(IObject) << std::endl;
				std::cout << "offsetof m_Name: " << offsetof(IObject, m_Name) << std::endl;
				std::cout << "offsetof m_DisplayName: " << offsetof(IObject, m_DisplayName) << std::endl;

				std::cout << "offsetof m_TickGroup: " << offsetof(IObject, m_TickGroup) << std::endl;
				std::cout << "offsetof bIsMarkedForDestruction: " << offsetof(IObject, bIsMarkedForDestruction) << std::endl;
			}
			{
				auto prop = IObject::StaticClass()->FindProperty("m_DisplayName");
				auto* sProp = static_cast<SProperty<IObject, std::string>*>(prop);

				// Print the stored offset
				std::cout << "stored m_Offset: " << sProp->m_Offset << std::endl;

				// Print what it should be
				IObject temp;
				char* base = (char*)&temp;
				std::cout << "actual offset: " << ((char*)&temp.m_DisplayName - base) << std::endl;

			}

			//{
			//	auto prop = IObject::StaticClass()->FindProperty("m_DisplayName");
			//	auto* sProp = static_cast<SProperty<IObject, std::string>*>(prop);

			//	// Print the raw bytes of MemberPtr
			//	unsigned char* mp = reinterpret_cast<unsigned char*>(&sProp->MemberPtr);
			//	std::cout << "MemberPtr raw bytes: ";
			//	for (int i = 0; i < sizeof(sProp->MemberPtr); i++)
			//		std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)mp[i] << " ";
			//	std::cout << std::endl;
			//	std::cout << "MemberPtr size: " << sizeof(sProp->MemberPtr) << std::endl;
			//}
			//{
			//	IObject temp;
			//	char* base = (char*)&temp;
			//	std::cout << "runtime offset of m_DisplayName: "
			//		<< ((char*)&temp.m_DisplayName - base) << std::endl;
			//}
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
