#include "Base/GameMode.h"

GameMode::GameMode()
{

}


void GameMode::Update()
{
	ClearBackground(RED);
}

void GameMode::SetName(std::string Name)
{
	this->m_Name = Name;
}

std::string GameMode::GetName()
{
	return this->m_Name;
}
