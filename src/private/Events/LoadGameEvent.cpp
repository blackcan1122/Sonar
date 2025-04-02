#include "Events/LoadGameEvent.h"

LoadGameEvent::~LoadGameEvent()
{
}

std::string LoadGameEvent::GetName() const
{
	return "LoadGameEvent";
}
