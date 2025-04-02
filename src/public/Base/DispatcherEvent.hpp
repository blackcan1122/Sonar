#pragma once
#include <memory>
#include "Base/Event.hpp"
#include "Base/EventDispatcher.hpp"

class DispatcherEvent : public Event
{
public:
	DispatcherEvent() = default;

	std::string GetName() const override;
	EventDispatcher* GetDispatcher();

	EventDispatcher* CurrentDispatcher;


};