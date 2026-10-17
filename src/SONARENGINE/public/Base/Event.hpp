#pragma once
#include "Base/Macros.h"
#include <typeindex>


class SONARENGINE_API IEvent
{
	ROOTBODY(IEvent)


public:

	virtual ~IEvent() = default;
};


class SONARENGINE_API Event : public IEvent
{
	AUTOBODY(Event, IEvent)


};