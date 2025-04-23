#pragma once
#include "Base/Core.h"
#include <typeindex>


class IEvent : public Object
{
	AUTOBODY(IEvent, Object)


public:

	virtual ~IEvent() = default;
};


class Event : public IEvent
{
	AUTOBODY(Event, IEvent)


};