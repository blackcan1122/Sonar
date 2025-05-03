#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"

class MouseEvent : public Event
{
public:
	MouseButton KeyPressed;
	Vector2 MousePos;
};