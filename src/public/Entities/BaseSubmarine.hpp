#pragma once
#include "Base/Core.h"
#include "Base/Entity.hpp"

class BaseSubmarine : public Entity
{
	AUTOBODY(BaseSubmarine, Entity)

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetSpeed(NavalUnits::Knot DesiredKnots);
	virtual void SetInitialSpeed(NavalUnits::Knot DesiredKnots);
	virtual void SetCourse(int Course);


protected:

	virtual void CalculateSpeed(float Deltatime) override;
	virtual void CalculateRotation(float Deltatime) override;

	virtual void Accel(float Deltatime) override;
	virtual void Turning() override;
};