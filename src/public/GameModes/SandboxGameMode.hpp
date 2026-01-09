#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "UI/WaterfallDisplay.hpp"
#include "UI/Map.hpp"
#include "Entities/Player.hpp"
#include "Base/EventDispatcher.hpp"
#include "UI/PlayerUI.hpp"
#include "UI/GridLayoutManager.hpp"
#include <list>

class Display;

class SandboxGameMode : public GameMode
{
public:
	SandboxGameMode();

	AUTOBODY(SandboxGameMode, GameMode)



	virtual void Update() override;
	virtual void SetName(std::string Name) override;
	virtual void BeginPlay() override;

	void OnMapClickedEvent(std::shared_ptr<IEvent> Event);
	
	// Display management callbacks
	void OnDeleteDisplay(SoftObjectPath<Display> display);
	void OnCreateDisplay(const SoftObjectPath<Display> InDisplay, const DisplaySpawnInfo InDisplaySpawnInfo);

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
	
	std::list<SoftObjectPath<Display>> m_AllDisplays;
};
