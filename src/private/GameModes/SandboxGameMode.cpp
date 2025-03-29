#include "GameModes/SandboxGameMode.hpp"

SandboxGameMode::SandboxGameMode()
{
	SetName("SandboxGameMode");
}

SandboxGameMode::~SandboxGameMode()
{
}

void SandboxGameMode::Update()
{
		ClearBackground(RED);

}

void SandboxGameMode::SetName(std::string Name)
{
	m_Name = Name;
}

std::string SandboxGameMode::GetName()
{
	return m_Name;
}
