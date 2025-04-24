#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/GameInstance.h"

class Player;

class PlayerUI : public BaseUI
{
	AUTOBODY(PlayerUI, BaseUI)

public:

	PlayerUI(SoftObjectPath<Player> Player);

	SoftObjectPath<Player> AssignedPlayer;

	std::shared_ptr<TextInputBox> CourseBox;
	std::shared_ptr<TextInputBox> SpeedBox;
	std::shared_ptr<TextInputBox> DepthBox;
	std::shared_ptr<TextInputBox> NameBox;

	void SetPosition(Vector2 NewPos);
	void SetNewPlayer(SoftObjectPath<Player> NewPlayer);
	virtual void Tick(float DeltaTime) override;
	virtual void Draw();

private:

	unsigned int Course = 0;
	int Speed = 0;
	unsigned int Depth = 0;
	Vector2 m_Position = { 0,0 };


};