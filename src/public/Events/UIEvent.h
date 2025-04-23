#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "UI/TextBox.h"


class UIEvent : public Event
{
	AUTOBODY(UIEvent, Event)
public:

	BaseUI* ClickedUIElement = nullptr;

	virtual void Tick(float DeltaTime) override {}

	std::string Payload;

};

