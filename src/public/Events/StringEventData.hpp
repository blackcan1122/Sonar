#pragma once
#include "Base/Core.h"
#include "Base/EventData.hpp"
#include "Base/Entity.hpp"


class StringEventData : public EventData
{
	AUTOBODY(StringEventData, EventData)
public:
	std::string String;

};