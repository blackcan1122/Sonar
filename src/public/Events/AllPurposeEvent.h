#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "UI/TextBox.h"
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

DECLARE_CLASS(AllPurposeEvent, Event)

public:

	virtual std::string GetName() const override;

	std::shared_ptr<IEventData> Payload = nullptr;

END_CLASS

