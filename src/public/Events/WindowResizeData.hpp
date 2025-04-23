#pragma once
#include "Base/Core.h"
#include "Base/Event.hpp"
#include "Base/EventData.hpp"


class WindowResizeData : public EventData
{
	AUTOBODY(WindowResizeData, EventData)
public:

	int width = 0;
	int height = 0;

};