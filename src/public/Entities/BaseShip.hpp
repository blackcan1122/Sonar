#pragma once
#include "Base/Core.h"
#include "Base/Entity.hpp"

DECLARE_CLASS(BaseShip, Entity)
public:
	virtual void Tick(float DeltaTime) override;

END_CLASS