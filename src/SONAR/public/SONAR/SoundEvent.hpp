#pragma once

#include "Base/Event.hpp"
#include "Base/NavalTypedefs.h"
#include "Base/SoftObject.hpp"

#include "raylib.h"

class Entity;

class SoundEvent : public Event
{
	AUTOBODY(SoundEvent, Event)

public:
	Vector2 SoundOrigin = { 0, 0 };
	NavalUnits::Hz SignalStrength = 0;
	SoftObjectPath<Entity> Sender;
};