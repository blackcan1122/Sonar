#pragma once
#include "Base/Event.hpp"
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"
#include <memory>

class AllPurposeEvent : public Event
{
	AUTOBODY(AllPurposeEvent, Event)

public:

	std::shared_ptr<IEventData> Payload = nullptr;

};

