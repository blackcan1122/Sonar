#pragma once

#include "Base/Event.hpp"
#include "SONAR/NavalUnits.hpp"
#include "Base/SoftObject.hpp"

#include "Rendering/Graphics.hpp"

class Entity;

class SoundEvent : public Event
{
	AUTOBODY(SoundEvent, Event)

public:
	Sonar::Vector2 SoundOrigin = { 0, 0 };
	NavalUnits::Hz SignalStrength = 0;
	SoftObjectPath<Entity> Sender;
};