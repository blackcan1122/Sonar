#pragma once
#include <memory>
#include "Base/Event.hpp"
#include "Base/EventDispatcher.hpp"

class DispatcherEvent : public Event
{
public:
	DispatcherEvent() = default;

	EventDispatcher* GetDispatcher();

	EventDispatcher* CurrentDispatcher;


};