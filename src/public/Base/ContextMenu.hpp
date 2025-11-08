#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/ContextMenuEntry.hpp"

class ContextMenu : public BaseUI
{
public:
	AUTOBODY(ContextMenu, BaseUI)
public:

	void Tick(float DeltaTime) override;
	void OnConstruct(const Vector2 Position);
	void OnDelete();
	virtual void OnMouseButtonPressed(MouseButton Key, Vector2 MousePos) override;

	bool GetIsConstructed() const;

	void AddMenuEntry(const ContextMenuEntry Entry);
	Rectangle Window;
	int PaddingY = 2;
	Vector2 MousePosWhenConstructed;


protected:

	void CalculateSize();
	void Draw() const;
	bool IsConstructed = false;
	std::vector<ContextMenuEntry> MenuEntries;
	Color BackgroundColor = DARKGRAY;

	float ElapsedLifetime = 0.f;
	float const CloseDelaySecs = 0.3f;
};