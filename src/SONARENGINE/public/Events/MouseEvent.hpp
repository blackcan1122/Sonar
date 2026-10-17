#pragma once
#include "Base/Event.hpp"

#include "Rendering/Graphics.hpp"

class SONARENGINE_API MouseEvent : public Event
{
	AUTOBODY(MouseEvent, Event)
public:
	Sonar::MouseButton KeyPressed;
	Sonar::Vector2 MousePos;
};