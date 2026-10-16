#pragma once
#include "Base/Event.hpp"

// TODO REFACTOR
class SaveGameEvent : public Event
{
public:
	virtual ~SaveGameEvent() override;

	//std::shared_ptr<TimeTrackerSave> SaveGame = nullptr;
};