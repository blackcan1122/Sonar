#pragma once
#include "Base/Core.h"
#include "Base/Entity.hpp"

class BaseSubmarine : public Entity
{
	AUTOBODY(BaseSubmarine)

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetSpeed(NavalUnits::Knot DesiredKnots);


protected:

	const float AccelerationRate = 2.f;
	const float DampeningRate = 0.01f;

	float SpeedChangeDelay = 0.2f;
	float SpeedChangeTimer = 0.0f;

	virtual void Accel();
};