#pragma once
#include "Base/Core.h"
#include "Base/Entity.hpp"

DECLARE_CLASS(BaseSubmarine, Entity)
public:
	virtual void Tick(float DeltaTime) override;
	virtual void Accel(float Amount);


	END_CLASS