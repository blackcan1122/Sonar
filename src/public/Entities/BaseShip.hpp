#pragma once
#include "Base/Core.h"
#include "Base/Entity.hpp"

class BaseShip : public Entity
{
	AUTOBODY(BaseShip, Entity)
public:
	virtual void Tick(float DeltaTime) override;

};