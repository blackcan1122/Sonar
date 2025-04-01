#include "GameModes/SandboxGameMode.hpp"
#include "Base/GameInstance.h"

SandboxGameMode::SandboxGameMode()
{
	SetName("SandboxGameMode");

	WaterfallDisplay = ObjectFactory.NewObject<Waterfall>(360, 360, 30);
	WaterfallDisplay.lock()->SetPosition(Vector2{0,0});
}

SandboxGameMode::~SandboxGameMode()
{
}

void SandboxGameMode::Update()
{
		ClearBackground(RED);
		GameMode::Update();

		if (IsKeyPressed(KEY_S))
		{
			WaterfallDisplay.lock()->MarkForDestruction();
		}
		if (IsKeyPressed(KEY_N))
		{
			WaterfallDisplay = ObjectFactory.NewObject<Waterfall>(360, 360, 30);
			WaterfallDisplay.lock()->SetPosition(Vector2{ 0,0 });
		}

		DrawFPS(20, 20);
}

void SandboxGameMode::SetName(std::string Name)
{
	m_Name = Name;
}

std::string SandboxGameMode::GetName()
{
	return m_Name;
}
