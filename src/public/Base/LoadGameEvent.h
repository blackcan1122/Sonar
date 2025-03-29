#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"

class LoadGameEvent : public Event<LoadGameEvent>
{
public:
	virtual ~LoadGameEvent() override;

	virtual std::string GetName() const override;

	GameMode* TimeCalcGameMode = nullptr;

};