#pragma once
#include "Base/Core.h"
#include <typeindex>

// also maybe introduce a EvenType DataType which could also be used to identify different events, instead the need of creating a different event for each use case

DECLARE_CLASS(IEvent, Object)

public:

	virtual ~IEvent() = default;
	virtual std::string GetName() const = 0;

END_CLASS


DECLARE_CLASS(Event, IEvent)

public:

	virtual std::string GetName() const = 0;

END_CLASS