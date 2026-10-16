#pragma once
#include "Base/EventData.hpp"


class WindowResizeData : public EventData
{
	AUTOBODY(WindowResizeData, EventData)
public:

	int width = 0;
	int height = 0;

};