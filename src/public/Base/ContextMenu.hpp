#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/ContextMenuEntry.hpp"

class ContextMenu : public BaseUI
{
public:

	void Tick(float DeltaTime) override;
	void TestTick();
	void OnConstruct(const Vector2 Position);
	void OnDelete();

	void AddMenuEntry(const ContextMenuEntry Entry);
	Rectangle Window;


protected:

	void CalculateSize();
	void Draw() const;
	bool IsConstructed = false;
	std::vector<ContextMenuEntry> MenuEntries;
	Color BackgroundColor = DARKGRAY;
};