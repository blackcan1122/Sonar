#pragma once
#include "Base/Core.h"
#include "Entities/NavalEntity.hpp"

class BaseSubmarine : public NavalEntity
{
	AUTOBODY_REFLECT(BaseSubmarine, NavalEntity)

public:
	virtual void Tick(float DeltaTime) override;


protected:

	virtual void CalculateSpeed(float Deltatime) override;
	virtual void CalculateRotation(float Deltatime) override;

	virtual void Accel(float Deltatime) override;
	virtual void Turning() override;
};