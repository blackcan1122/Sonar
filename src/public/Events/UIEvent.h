#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/TextBox.h"

DECLARE_CLASS(UIEvent, Event)
public:

	virtual std::string GetName() const override;

	BaseUI* ClickedUIElement = nullptr;

	std::string Payload;

END_CLASS

