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
	void SetUpEvents();
	std::string GetName() override;

	Texture2D Background;

protected:
	float m_DeltaTime = 0;

	std::shared_ptr<EventDispatcher> UIDispatcher;

	int Height;
	int Width;

#if DEBUG
	std::shared_ptr<Button> Sandbox;
#endif
	std::shared_ptr<Button> StartGame;
	std::shared_ptr<Button> Option;
	std::shared_ptr<Button> Exit;


	

};
