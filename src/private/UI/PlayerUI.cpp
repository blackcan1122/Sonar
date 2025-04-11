#include "UI/PlayerUI.hpp"
#include "Entities/Player.hpp"
#include "UI/Button.h"
#include "UI/TextBox.h"

PlayerUI::PlayerUI(SoftObjectPath<Player> Player)
	:AssignedPlayer(Player)
{
	CourseBox = std::make_shared<TextInputBox>();
	Rectangle CourseRectangle{ m_Position.x + 0, m_Position.y + 0, 100,50 };
	CourseBox->Construct(CourseRectangle, BLANK).CanBeEdited(false);
}


void PlayerUI::SetPosition(Vector2 NewPos)
{
	m_Position = NewPos;
	CourseBox->UpdatePosition({ m_Position.x + 0, m_Position.y + 0 });
}

void PlayerUI::SetNewPlayer(SoftObjectPath<Player> NewPlayer)
{
	AssignedPlayer = NewPlayer;
}

void PlayerUI::Tick(float DeltaTime)
{
	CourseBox->SetInitialText(std::to_string(AssignedPlayer.TryLoad()->GetEntityRotation()));
	CourseBox->Tick(DeltaTime);
}

void PlayerUI::Draw()
{
	
}