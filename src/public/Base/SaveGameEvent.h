#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"


class SaveGameEvent : public Event<SaveGameEvent>
{
public:
	virtual ~SaveGameEvent() override;

	virtual std::string GetName() const override;

	//std::shared_ptr<TimeTrackerSave> SaveGame = nullptr;
};