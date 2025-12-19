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

class SandboxGameMode : public GameMode
{
public:
	SandboxGameMode();



	virtual void Update() override;
	virtual void SetName(std::string Name) override;
	virtual void BeginPlay() override;

	void OnMapClickedEvent(std::shared_ptr<IEvent> Event);

	void DrawFocusPlayer();
	
	// Display management callbacks
	void OnDeleteDisplay(Display* display);
	void OnCreateDisplay(const GridCell& cell, const DisplaySpawnInfo& spawnInfo);

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
	
	// Track all waterfall displays for dynamic add/remove
	std::list<SoftObjectPath<Waterfall>> m_WaterfallDisplays;
	
	// Counter for unique display frequencies
	int m_NextWaterfallFrequency = 100;
};
