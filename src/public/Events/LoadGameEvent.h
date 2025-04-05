#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"

// TODO REFACTOR
class LoadGameEvent : public Event
{
public:
	virtual ~LoadGameEvent() override;

	GameMode* TimeCalcGameMode = nullptr;

};