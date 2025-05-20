#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/ContextMenuEntry.hpp"
#include "Base/SoftObject.hpp"
#include <any>

class ContextMenu;

class ContextMenuEntry : public BaseUI
{
public:
	void Construct();
	void OnClick();
	void SetCallback(std::function<void(ContextMenuEntry* Self)> NewCallback);
	
	int FontSize = 8;
	int MeasuredText = 0;

	Rectangle ContextMenuEntryRec;

protected:

	std::function<void (ContextMenuEntry* Self)> Callback;

	virtual void Tick(float DeltaTime) override {};
};