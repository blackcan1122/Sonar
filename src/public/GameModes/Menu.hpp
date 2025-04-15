#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "UI/Button.h"
#include "Base/Texture2DWrap.hpp"

class MenuMode : public GameMode
{
public:
	MenuMode();
	~MenuMode();

	void Update() override;
	void BeginPlay() override;
	void SetName(std::string Name) override;
	void SetUpEvents();
	std::string GetName() override;

	SharedTexture2D Background;

protected:
	float m_DeltaTime = 0;

	TextureResource* BackgroundResource = nullptr;

	std::shared_ptr<EventDispatcher> UIDispatcher;

	Music MenuMusic;
	int Height;
	int Width;

#if DEBUG
	SoftObjectPath<Button> Sandbox;
#endif
	SoftObjectPath<Button> StartGame;
	SoftObjectPath<Button> Option;
	SoftObjectPath<Button> Exit;

};
