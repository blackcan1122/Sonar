#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "UI/TextBox.h"

DECLARE_CLASS(UIEvent, Event)
public:

	virtual std::string GetName() const override;

	BaseUI* ClickedUIElement = nullptr;

	virtual void Tick(float DeltaTime) override;

	std::string Payload;

END_CLASS

