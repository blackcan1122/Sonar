#pragma once
#include "Base/Core.h"
#include "Base/Entity.hpp"

class BaseSubmarine : public Entity
{
	AUTOBODY(BaseSubmarine, Entity)

public:
	virtual void Tick(float DeltaTime) override;


protected:

	virtual void CalculateSpeed(float Deltatime) override;
	virtual void CalculateRotation(float Deltatime) override;

	virtual void Accel(float Deltatime) override;
	virtual void Turning() override;
};