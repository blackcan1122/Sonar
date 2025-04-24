#include "UI/PlayerUI.hpp"
#include "Entities/Player.hpp"
#include "UI/Button.h"
#include "UI/TextBox.h"

PlayerUI::PlayerUI(SoftObjectPath<Player> Player)
	:AssignedPlayer(Player)
{
	CourseBox = std::make_shared<TextInputBox>();
	Rectangle CourseRectangle{ m_Position.x, m_Position.y, 100,50 };
	CourseBox->Construct(CourseRectangle, BLANK).CanBeEdited(false);

	SpeedBox = std::make_shared<TextInputBox>();
	Rectangle SpeedRect{ m_Position.x, m_Position.y, 100,50 };
	SpeedBox->Construct(SpeedRect, BLANK).CanBeEdited(false);
	NameBox = std::make_shared<TextInputBox>();
	Rectangle NameRect{ m_Position.x, m_Position.y, 100,50 };
	NameBox->Construct(NameRect, BLANK).CanBeEdited(false);
	NameBox->SetInitialText(AssignedPlayer.TryLoad()->GetDisplayName().c_str());

}


void PlayerUI::SetPosition(Vector2 NewPos)
{
	m_Position = NewPos;
	CourseBox->UpdatePosition({ m_Position.x, m_Position.y });
	SpeedBox->UpdatePosition({ m_Position.x, m_Position.y + 15 });
	NameBox->UpdatePosition({ m_Position.x + 0, m_Position.y + 30 });
}

void PlayerUI::SetNewPlayer(SoftObjectPath<Player> NewPlayer)
{
	AssignedPlayer = NewPlayer;
}

void PlayerUI::Tick(float DeltaTime)
{
	if (AssignedPlayer.TryLoad())
	{ 
		CourseBox->SetInitialText("Course: " + std::to_string(static_cast<int>(AssignedPlayer.TryLoad()->GetEntityRotation())));
		CourseBox->Tick(DeltaTime);
		SpeedBox->SetInitialText("Speed: " + std::to_string(static_cast<int>(AssignedPlayer.TryLoad()->GetCurrentSpeed())));
		SpeedBox->Tick(DeltaTime);
		NameBox->Tick(DeltaTime);
	}
}

void PlayerUI::Draw()
{
	
}