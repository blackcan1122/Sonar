#pragma once
#include "Base/Core.h"
class BaseUI
{

public:

	// Methods

	BaseUI() = default;
	~BaseUI() = default;

	virtual void Update() = 0;


private:

	// Member

	Color m_BackgroundColor;
};

