#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"

SandboxGameMode::SandboxGameMode()
{
	SetName("SandboxGameMode");

	WaterfallDisplay = ObjectFactory.NewObject<Waterfall>(360, 300, 10);
	WaterfallDisplay.lock()->SetPosition(Vector2{0,0});

	WaterfallDisplay2 = ObjectFactory.NewObject<Waterfall>(360, 300, 60);
	WaterfallDisplay2.lock()->SetPosition(Vector2{ 0,310 });
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
