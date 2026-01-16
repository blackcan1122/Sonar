#pragma once
#include "Base/Core.h"

class BaseUI : public Object
{
	AUTOBODY(BaseUI, Object)

public:

	// Methods

	BaseUI() { m_TickGroup.SetTickGroup(ETickGroup::Rendering); };
	virtual void MarkForDestruction() override;
	virtual ~BaseUI() = default;


protected:
};

