#include "GameModes/SandboxGameMode.hpp"

SandboxGameMode::SandboxGameMode()
{
	SetName("SandboxGameMode");

	WaterfallDisplay = std::make_shared<Waterfall>(200, 200);
	WaterfallDisplay->SetPosition(Vector2{400,400});
}

SandboxGameMode::~SandboxGameMode()
{
}

void SandboxGameMode::Update()
{
		ClearBackground(RED);
		DrawFPS(20, 20);
		WaterfallDisplay->Update();
		WaterfallDisplay->Draw();
		WaterfallDisplay->RenderToMainBuffer();

}

void SandboxGameMode::SetName(std::string Name)
{
	m_Name = Name;
}

std::string SandboxGameMode::GetName()
{
	return m_Name;
}
