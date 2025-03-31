#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "Base/Button.h"

class OptionsMode : public GameMode
{
public:
	OptionsMode();
	~OptionsMode();

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

	std::shared_ptr<Button> Back;
	std::shared_ptr<Button> Apply;


	

};
