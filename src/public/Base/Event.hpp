#pragma once
#include "Base/Core.h"
#include <typeindex>


DECLARE_CLASS(IEvent, Object)

public:

	virtual ~IEvent() = default;
END_CLASS


DECLARE_CLASS(Event, IEvent)

public:


END_CLASS