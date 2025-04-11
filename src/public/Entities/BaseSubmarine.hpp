#pragma once
#include "Base/Core.h"
#include "Base/Entity.hpp"

class BaseSubmarine : public Entity
{
	AUTOBODY(BaseSubmarine)

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetSpeed(NavalUnits::Knot DesiredKnots);
	virtual void SetInitialSpeed(NavalUnits::Knot DesiredKnots);
	virtual void SetCourse(int Course);


protected:

	virtual void CalculateSpeed(float Deltatime);
	virtual void CalculateRotation(float Deltatime);

	const float AccelerationRate = 2.f;
	const float DampeningRate = 0.01f;

	float SpeedChangeDelay = 0.2f;
	float SpeedChangeTimer = 0.0f;
	float RotationChangeTimer = 0.0f;

	float BaseTurningRate = 6.f;

	virtual void Accel(float Deltatime);
	virtual void Turning();
};