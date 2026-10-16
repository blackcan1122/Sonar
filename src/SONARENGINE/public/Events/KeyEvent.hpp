#pragma once
#include "Base/Event.hpp"

#include "raylib.h"

class KeyEvent : public Event
{
public:
	KeyboardKey KeyPressed;
	Vector2 MousePos;
};