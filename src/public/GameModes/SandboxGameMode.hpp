#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "UI/WaterfallDisplay.hpp"
#include "UI/Map.hpp"
#include "Entities/Player.hpp"
#include "Base/EventDispatcher.hpp"
#include "UI/PlayerUI.hpp"

class SandboxGameMode : public GameMode
{
public:
	SandboxGameMode();
	~SandboxGameMode();



	void Update() override;
	void SetName(std::string Name) override;
	void BeginPlay() override;
	void OnMapClickedEvent(std::shared_ptr<IEvent> Event);



	void DrawFocusPlayer();

	std::string GetName() override;

protected:

	float m_DeltaTime = 0;

	SoftObjectPath<Waterfall> WaterfallDisplay;
	SoftObjectPath<Waterfall> WaterfallDisplay2;
	SoftObjectPath<Map> MapDisplay;
	SoftObjectPath<Player> PlayerOne;
	SoftObjectPath<BaseSubmarine> OtherSub;

	SoftObjectPath<Entity> FocusedUnit;

	SoftObjectPath<PlayerUI> m_PlayerUI;

	SoftObjectPath<World> CurrentWorld;

};
