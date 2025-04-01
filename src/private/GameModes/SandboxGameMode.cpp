#include "GameModes/SandboxGameMode.hpp"

SandboxGameMode::SandboxGameMode()
{
	SetName("SandboxGameMode");

	WaterfallDisplay = std::make_shared<Waterfall>(360, 600, 120);
	WaterfallDisplay->SetPosition(Vector2{0,0});
}

SandboxGameMode::~SandboxGameMode()
{
}

void SandboxGameMode::Update()
{
		ClearBackground(RED);
		
		float DeltaTime = GetFrameTime();

		WaterfallDisplay->Tick(DeltaTime);
		WaterfallDisplay->Draw();
		WaterfallDisplay->RenderToMainBuffer();

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
