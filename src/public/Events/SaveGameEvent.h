#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"

// TODO REFACTOR
class SaveGameEvent : public Event
{
public:
	virtual ~SaveGameEvent() override;

	virtual std::string GetName() const override;

	//std::shared_ptr<TimeTrackerSave> SaveGame = nullptr;
};