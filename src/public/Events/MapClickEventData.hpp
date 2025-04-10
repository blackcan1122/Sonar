#pragma once
#include "Base/Core.h"
#include "Base/EventData.hpp"

DECLARE_CLASS(MapClickEventData, EventData)
public:
	// TODO: Should be changed to use SoftObjectPath
	SoftObjectPath<IObject> ClickedObject;

END_CLASS