#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/TextBox.h"

class UIEvent : public Event<UIEvent>
{
public:

	virtual std::string GetName() const override;

	BaseUI* ClickedUIElement = nullptr;

};

