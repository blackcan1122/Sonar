#pragma once
#include "Base/Core.h"
#include "Base/EventData.hpp"
#include "Base/Entity.hpp"

DECLARE_CLASS(StringEventData, EventData)
public:
	// TODO: Should be changed to use SoftObjectPath
	std::string String;

	END_CLASS