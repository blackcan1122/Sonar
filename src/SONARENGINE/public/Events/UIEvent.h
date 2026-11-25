#pragma once
#include "Base/Event.hpp"

#include <string>

class BaseUI;


class SONARENGINE_API UIEvent : public Event
{
	AUTOBODY(UIEvent, Event)
public:

	BaseUI* ClickedUIElement = nullptr;

	std::string Payload;

};

