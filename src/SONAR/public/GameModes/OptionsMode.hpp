#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "UI/Button.h"
#include "Base/Texture2DWrap.hpp"

class OptionsMode : public GameMode
{
AUTOBODY(OptionsMode, GameMode)
public:
	OptionsMode();
	~OptionsMode();

	void Update() override;
	virtual void BeginPlay() override;
	void SetName(std::string Name) override;
	void SetUpEvents();
	std::string GetName() override;

	SharedTexture2D Background;

protected:
	float m_DeltaTime = 0;

	std::shared_ptr<EventDispatcher> UIDispatcher;

	int Height;
	int Width;

	std::shared_ptr<Button> Back;
	std::shared_ptr<Button> Apply;


	

};
