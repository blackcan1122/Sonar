#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/SoftObject.hpp"

class SoundEvent : public Event
{
	AUTOBODY(SoundEvent, Event)

public:

	Vector2 SoundOrigin = {0,0};
	NavalUnits::Hz SignalStrength = 0;
	SoftObjectPath<Entity> Sender;
};