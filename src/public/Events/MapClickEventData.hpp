#pragma once
#include "Base/Core.h"
#include "Base/EventData.hpp"
#include "Base/Entity.hpp"

DECLARE_CLASS(MapClickEventData, EventData)
public:
	// TODO: Should be changed to use SoftObjectPath
	SoftObjectPath<Entity> ClickedObject;

END_CLASS