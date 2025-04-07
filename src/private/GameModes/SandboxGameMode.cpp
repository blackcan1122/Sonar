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

	PlayerOne = m_ObjectFactory.NewObject<Player>();
	PlayerOne.TryLoad()->SetEntityLocation(Vector2{ 0,0 });
	MapDisplay.TryLoad()->AddObjectToDraw(PlayerOne.TryLoad());

	PlayerTwo = m_ObjectFactory.NewObject<Player>();
	PlayerTwo.TryLoad()->SetEntityLocation(Vector2{ 800,200 });
	MapDisplay.TryLoad()->AddObjectToDraw(PlayerTwo.TryLoad());
}

SandboxGameMode::~SandboxGameMode()
{
}

void SandboxGameMode::Update()
{
		ClearBackground(RED);


		if (IsKeyPressed(KEY_S))
		{
			DestroyObjectExplicitly(PlayerOne);
		}
		if (IsKeyPressed(KEY_N))
		{
			WaterfallDisplay = m_ObjectFactory.NewObject<Waterfall>(360, 360, 30);
			WaterfallDisplay.TryLoad()->SetPosition(Vector2{0,0});
		}


		if (auto tempPlayer = PlayerOne.TryLoad())
		{
			tempPlayer->Accel(0.2f);
		}

		DrawCircleLines(600, 600, 15, BLUE);

		GameMode::Update();

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
