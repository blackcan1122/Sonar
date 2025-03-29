#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/EventData.hpp"

class WindowResizeData : public EventData<WindowResizeData>
{
public:

	int width = 0;
	int height = 0;
};