#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "Base/Button.h"

class MenuMode : public GameMode
{
public:
	MenuMode();
	~MenuMode();

	void Update() override;
	void SetName(std::string Name) override;
	std::string GetName() override;

protected:
	float m_DeltaTime = 0;

#if DEBUG
	std::shared_ptr<Button> Sandbox;
#endif
	std::shared_ptr<Button> StartGame;
	std::shared_ptr<Button> Option;
	std::shared_ptr<Button> Exit;


	

};
