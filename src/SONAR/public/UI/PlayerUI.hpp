#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/GameInstance.h"
#include "Base/SoftObject.hpp"
#include "UI/TextBox.h"


class SubmarinePlayer;

class PlayerUI : public BaseUI
{
	AUTOBODY(PlayerUI, BaseUI)

public:

	PlayerUI(SoftObjectPath<SubmarinePlayer> Player);

	SoftObjectPath<SubmarinePlayer> AssignedPlayer;

	std::shared_ptr<TextInputBox> CourseBox;
	std::shared_ptr<TextInputBox> SpeedBox;
	std::shared_ptr<TextInputBox> DepthBox;
	std::shared_ptr<TextInputBox> NameBox;

	void SetPosition(Sonar::Vector2 NewPos);
	void SetNewPlayer(SoftObjectPath<SubmarinePlayer> NewPlayer);
	virtual void Tick(float DeltaTime) override;
	virtual void Draw();

private:

	unsigned int Course = 0;
	int Speed = 0;
	unsigned int Depth = 0;
	Sonar::Vector2 m_Position = { 0,0 };


};