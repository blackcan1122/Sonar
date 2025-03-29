#include "Base/DispatcherEvent.hpp"

std::string DispatcherEvent::GetName() const
{
	return "DispatcherEvent";
}

EventDispatcher* DispatcherEvent::GetDispatcher()
{
	return CurrentDispatcher;
}
