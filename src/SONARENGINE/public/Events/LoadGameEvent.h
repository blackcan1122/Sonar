#pragma once
#include "Base/Event.hpp"

class GameMode;

// TODO REFACTOR
class LoadGameEvent : public Event
{
public:
	virtual ~LoadGameEvent() override;

	GameMode* TimeCalcGameMode = nullptr;

};