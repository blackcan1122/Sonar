#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/EventData.hpp"

DECLARE_CLASS(WindowResizeData, EventData)
public:

	int width = 0;
	int height = 0;

END_CLASS