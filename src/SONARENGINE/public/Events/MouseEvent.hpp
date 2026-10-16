#pragma once
#include "Base/Event.hpp"

#include "raylib.h"

class MouseEvent : public Event
{
public:
	MouseButton KeyPressed;
	Vector2 MousePos;
};