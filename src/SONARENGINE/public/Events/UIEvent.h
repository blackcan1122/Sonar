#pragma once
#include "Base/Event.hpp"

#include <string>

class BaseUI;


class UIEvent : public Event
{
	AUTOBODY(UIEvent, Event)
public:

	BaseUI* ClickedUIElement = nullptr;

	std::string Payload;

};

