#pragma once
#include "Base/Core.h"

DECLARE_CLASS(BaseUI, Object)

public:

	// Methods

	BaseUI() = default;
	~BaseUI() = default;

	virtual void Update() = 0;


private:

	// Member

	Color m_BackgroundColor;

END_CLASS

