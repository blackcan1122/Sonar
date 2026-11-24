#pragma once
#include "Base/Event.hpp"

#include "Base/SonarTypes.hpp"

class SONARENGINE_API MouseEvent : public Event
{
	AUTOBODY(MouseEvent, Event)
public:
	Sonar::MouseButton KeyPressed;
	Sonar::Vector2 MousePos;
};