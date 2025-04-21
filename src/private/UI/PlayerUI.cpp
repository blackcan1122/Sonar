#include "UI/PlayerUI.hpp"
#include "Entities/Player.hpp"
#include "UI/Button.h"
#include "UI/TextBox.h"

PlayerUI::PlayerUI(SoftObjectPath<Player> Player)
	:AssignedPlayer(Player)
{
	CourseBox = std::make_shared<TextInputBox>();
	Rectangle CourseRectangle{ m_Position.x, m_Position.y + 0, 100,50 };
	CourseBox->Construct(CourseRectangle, BLANK).CanBeEdited(false);

	SpeedBox = std::make_shared<TextInputBox>();
	Rectangle SpeedRect{ m_Position.x, m_Position.y + 0, 100,50 };
	SpeedBox->Construct(SpeedRect, BLANK).CanBeEdited(false);

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
	if (AssignedPlayer.TryLoad())
	{ 
		CourseBox->SetInitialText(std::to_string(AssignedPlayer.TryLoad()->GetEntityRotation()));
		CourseBox->Tick(DeltaTime);
		SpeedBox->SetInitialText(std::to_string(AssignedPlayer.TryLoad()->GetCurrentSpeed()));
		SpeedBox->Tick(DeltaTime);

		DrawText(AssignedPlayer.TryLoad()->GetDisplayName().c_str(), m_Position.x, m_Position.y + 10, 12, WHITE);
	}
}

void PlayerUI::Draw()
{
	
}