#pragma once
#include "Base/Core.h"
#include "Base/EventData.hpp"
#include "Base/Entity.hpp"


class MapClickEventData : public EventData
{
	AUTOBODY(MapClickEventData, EventData)
public:
	// TODO: Should be changed to use SoftObjectPath
	SoftObjectPath<Entity> ClickedObject;

};