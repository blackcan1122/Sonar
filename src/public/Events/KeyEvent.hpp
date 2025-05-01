#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"

class KeyEvent : public Event
{
public:
	KeyboardKey KeyPressed;
	Vector2 MousePos;
};