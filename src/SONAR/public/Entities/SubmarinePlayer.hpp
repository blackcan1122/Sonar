#pragma once

#include "Base/Core.h"
#include "Base/Entity.hpp"
#include "Entities/BaseSubmarine.hpp"

class Waterfall;

class SubmarinePlayer : public BaseSubmarine
{
	AUTOBODY(SubmarinePlayer, BaseSubmarine)

public:
	virtual void Tick(float DeltaTime) override;

	std::weak_ptr<Waterfall> WaterfallDisplay;
};