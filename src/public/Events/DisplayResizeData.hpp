#pragma once
#include "Base/Core.h"
#include "Base/EventData.hpp"
#include "Base/Entity.hpp"
#include "Base/SoftObject.hpp"

class Display;

class DisplayResizeData : public EventData
{
	AUTOBODY(DisplayResizeData, EventData)
public:
	SoftObjectPath<Display> DisplayToResize;
	int NewWidth = 0;
	int NewHeight = 0;

	int OldWidth = 0;
	int OldHeight = 0;

	bool bIsResizeComplete = false;
};