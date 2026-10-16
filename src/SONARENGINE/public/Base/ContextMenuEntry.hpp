#pragma once
#include "Base/BaseUI.h"

#include <functional>

class ContextMenu;

class SONARENGINE_API ContextMenuEntry : public BaseUI
{
public:
	void Construct();
	void OnClick();
	void SetCallback(std::function<void(ContextMenuEntry* Self)> NewCallback);
	
	int FontSize = 14;
	int MeasuredText = 0;
	bool CloseOnClick = true;

	Rectangle ContextMenuEntryRec;

protected:

	std::function<void (ContextMenuEntry* Self)> Callback;

	virtual void Tick(float DeltaTime) override {};
};