#pragma once
#include "Base/EventData.hpp"
#include "Base/Entity.hpp"

#include <string>


class StringEventData : public EventData
{
	AUTOBODY(StringEventData, EventData)
public:
	std::string String;

};