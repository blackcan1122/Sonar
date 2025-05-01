#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "UI/TextBox.h"
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

class AllPurposeEvent : public Event
{
	AUTOBODY(AllPurposeEvent, Event)

public:

	std::shared_ptr<IEventData> Payload = nullptr;

};

