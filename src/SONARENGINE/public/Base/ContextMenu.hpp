#pragma once
#include "Base/BaseUI.h"
#include "Base/ContextMenuEntry.hpp"

#include <functional>
#include <vector>

class SONARENGINE_API ContextMenu : public BaseUI
{
public:
	AUTOBODY(ContextMenu, BaseUI)
public:

	void Tick(float DeltaTime) override;
	void OnConstruct(const Sonar::Vector2 Position);
	void OnDelete();
	void AddOnCloseCallback(std::function<void()> Callback) { if (Callback) OnCloseCallbacks.push_back(Callback); };
	void ClearMenuEntries();
	virtual void OnMouseButtonPressed(Sonar::MouseButton Key, Sonar::Vector2 MousePos) override;

	bool GetIsConstructed() const;

	void AddMenuEntry(const ContextMenuEntry Entry);
	Sonar::Rectangle Window;
	int PaddingY = 2;
	Sonar::Vector2 MousePosWhenConstructed;


protected:

	void CalculateSize();
	void Draw() const;
	bool IsConstructed = false;
	std::vector<ContextMenuEntry> MenuEntries;
	Sonar::Color BackgroundColor = Sonar::DARKGRAY;

	std::vector<std::function<void()>> OnCloseCallbacks;

	float ElapsedLifetime = 0.f;
	float const CloseDelaySecs = 0.3f;
};