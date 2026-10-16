#pragma once
#include "Base/Macros.h"
#include <typeindex>


class IEvent
{
	ROOTBODY(IEvent)


public:

	virtual ~IEvent() = default;
};


class Event : public IEvent
{
	AUTOBODY(Event, IEvent)


};