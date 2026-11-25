#pragma once
#include "Base/EventData.hpp"
#include "Base/SoftObject.hpp"

class Display;

class SONARENGINE_API DisplayMoveData : public EventData
{
	AUTOBODY(DisplayMoveData, EventData)
public:
	SoftObjectPath<Display> DisplayToMove;

	float NewX = 0.0f;
	float NewY = 0.0f;

	float MouseX = 0.0f;
	float MouseY = 0.0f;

	float DisplayWidth = 0.0f;
	float DisplayHeight = 0.0f;

	bool bIsMoveComplete = false;
};
