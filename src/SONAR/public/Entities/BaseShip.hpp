#pragma once
#include "Base/Core.h"
#include "Entities/NavalEntity.hpp"

class BaseShip : public NavalEntity
{
	AUTOBODY(BaseShip, NavalEntity)
public:
	virtual void Tick(float DeltaTime) override;

};