#pragma once
#include "Base/Event.hpp"

#include "Base/SonarTypes.hpp"

class SONARENGINE_API KeyEvent : public Event
{
	AUTOBODY(KeyEvent, Event)
public:
	Sonar::KeyboardKey KeyPressed;
	Sonar::Vector2 MousePos;
};