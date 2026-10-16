#pragma once
#include "Base/Object.hpp"

class SONARENGINE_API BaseUI : public Object
{
	AUTOBODY(BaseUI, Object)

public:

	// Methods

	BaseUI() { m_TickGroup.SetTickGroup(ETickGroup::Rendering); };
	virtual void MarkForDestruction() override;
	virtual ~BaseUI() = default;


protected:
};

