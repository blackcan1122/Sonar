#pragma once
#include "Base/EventData.hpp"
#include "Base/Entity.hpp"
#include "Base/SoftObject.hpp"

class Display;

class SONARENGINE_API DisplayResizeData : public EventData
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