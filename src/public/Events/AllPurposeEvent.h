#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/TextBox.h"
#include "Base/EventData.hpp"
#include "Events/WindowResizeData.hpp"

class AllPurposeEvent : public Event<AllPurposeEvent>
{
public:

	virtual std::string GetName() const override;

	std::shared_ptr<IEventData> Payload = nullptr;

};

