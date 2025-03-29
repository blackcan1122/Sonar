#pragma once
#include <memory>
//#include "Tickable.h"
#include "Base/Event.hpp"
#include "Base/EventDispatcher.hpp"

class DispatcherEvent : public Event<DispatcherEvent>
{
public:
	DispatcherEvent() = default;

	std::string GetName() const override;
	EventDispatcher* GetDispatcher();

	EventDispatcher* CurrentDispatcher;


};