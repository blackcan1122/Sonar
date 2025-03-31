#include "Events/SaveGameEvent.h"

SaveGameEvent::~SaveGameEvent()
{
}

std::string SaveGameEvent::GetName() const
{
	return "SaveGameEvent";
}
