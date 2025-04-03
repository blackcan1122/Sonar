#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"

/*
* TODO:
*	- Give IObjects a unique tag, and when not defined by the User, should auto generate a Tag,
*		so it can be used to iterate over the GameMode Object Registry, and doesn't have to create a intermediate gamemode member
*	- Player Implementation duh
*	- World Scale and Coordinate System
*/

SandboxGameMode::SandboxGameMode()
{
	SetName("SandboxGameMode");

	WaterfallDisplay = ObjectFactory.NewObject<Waterfall>(360, 300, 10);
	WaterfallDisplay.lock()->SetPosition(Vector2{0,0});

	WaterfallDisplay2 = ObjectFactory.NewObject<Waterfall>(360, 300, 60);
	WaterfallDisplay2.lock()->SetPosition(Vector2{ 0,310 });

	MapDisplay = ObjectFactory.NewObject<Map>(400, 400);
	MapDisplay.lock()->SetPosition(Vector2{ 400,100 });

	PlayerOne = ObjectFactory.NewObject<Player>();
	MapDisplay.lock()->AddObjectToDraw(PlayerOne);
}

SandboxGameMode::~SandboxGameMode()
{
}

void SandboxGameMode::Update()
{
		ClearBackground(RED);

		auto MyDisplay = WaterfallDisplay.lock();

		if (IsKeyPressed(KEY_S))
		{
			WaterfallDisplay.lock()->MarkForDestruction();
		}
		if (IsKeyPressed(KEY_N))
		{
			WaterfallDisplay = ObjectFactory.NewObject<Waterfall>(360, 360, 30);
			WaterfallDisplay.lock()->SetPosition(Vector2{ 0,0 });
		}

		if (IsKeyDown(KEY_D))
		{
			MyDisplay = nullptr;
		}

		{
			PlayerOne.lock()->Accel(0.2f);
		}

		DrawCircleLines(600, 600, 15, BLUE);

		GameMode::Update();
#if DEBUG
		DrawFPS(GameInstance::GetInstance()->GetWindowProperties().ScreenWidth - 100, 20);
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
