#pragma once
#include "Base/Core.h"

class BaseUI : public Object
{
	AUTOBODY(BaseUI, Object)

public:

	// Methods

	BaseUI() = default;
	virtual void MarkForDestruction() override;
	virtual ~BaseUI() = default;


private:

};

