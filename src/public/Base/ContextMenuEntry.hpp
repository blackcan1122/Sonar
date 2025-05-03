#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/ContextMenuEntry.hpp"

class ContextMenuEntry : public BaseUI
{
public:
	void Construct();
	void OnClick();
	void SetCallback(void(*Function)(void));
	
	int FontSize = 8;
	int MeasuredText = 0;

protected:

	void(*OnClickCallback)(void);

	virtual void Tick(float DeltaTime) override {};
};