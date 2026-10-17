#pragma once
#include "Base/BaseUI.h"
#include "Base/EventDispatcher.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/DisplayResizeData.hpp"
#include "Events/DisplayMoveData.hpp"
#include "Rendering/Graphics.hpp"

#include <memory>
#include <mutex>
#include <optional>

class SONARENGINE_API Display : public BaseUI
{
	AUTOBODY(Display, BaseUI)
public:
	Display(int Width, int Height);
	virtual ~Display();

	virtual void Draw();
	void BeginDisplayDraw();
	void EndDisplayDraw();
	virtual void MarkForDestruction() override;

	virtual void Initialize() override;

	void SetPosition(Sonar::Vector2 NewPosition);
	void RenderToMainBuffer();

	virtual void ResizeDisplay(int NewWidth, int NewHeight);

	Sonar::Vector2 GetSize() const { return { DestinationRect.width, DestinationRect.height }; }
	int GetWidth() const { return static_cast<int>(DestinationRect.width); }
	int GetHeight() const { return static_cast<int>(DestinationRect.height); }
	Sonar::Rectangle GetDisplayedRect() const;

	void SetResizable(bool bCanResize) { bIsResizable = bCanResize; }
	bool IsResizable() const { return bIsResizable; }

	void SetMovable(bool bCanMove) { bIsMovable = bCanMove; }
	bool IsMovable() const { return bIsMovable; }
	bool IsManipulatingDisplay() const { return bIsResizing || bIsMoving; }

	void SetMinSize(int MinWidth, int MinHeight) { m_MinWidth = MinWidth; m_MinHeight = MinHeight; }

	virtual void Tick(float DeltaTime) override;

	Sonar::Rectangle GetDestinationRect() const { return DestinationRect; }

	EventDispatcher OnResize;
	EventDispatcher OnMove;
	void SetDisplayPreviewRect(const Sonar::Rectangle& PreviewRect);
	void ClearDisplayPreviewRect();

protected:
	mutable std::mutex m_ResizeMutex;

	void HandleResizeInteraction();
	void HandleMoveInteraction();
	void DrawResizeHandle();
	void DrawMoveHandle();
	Sonar::Rectangle GetResizeHandleRect() const;
	Sonar::Rectangle GetMoveHandleRect() const;

private:
	Sonar::RenderTexture2D ActiveRenderTarget;
protected:

	Sonar::Rectangle SourceRect;
	Sonar::Rectangle DestinationRect;

	std::shared_ptr<AllPurposeEvent> ResizeEvent;
	std::shared_ptr<AllPurposeEvent> ResizeEventComplete;

	std::shared_ptr<AllPurposeEvent> MoveEvent;
	std::shared_ptr<AllPurposeEvent> MoveEventComplete;

	std::shared_ptr<DisplayResizeData> ResizeData;
	std::shared_ptr<DisplayMoveData> MoveData;

	bool bIsResizable = true;
	bool bIsResizing = false;
	bool bIsResizeFinished = true;
	Sonar::Vector2 m_ResizeStartMousePos = { 0, 0 };
	Sonar::Vector2 m_ResizeStartSize = { 0, 0 };
	int m_ResizeHandleSize = 15;
	int m_MinWidth = 100;
	int m_MinHeight = 100;
	std::optional<Sonar::Rectangle> m_DisplayPreviewRect;

	bool bIsMovable = true;
	bool bIsMoving = false;
	Sonar::Vector2 m_MoveStartMousePos = { 0, 0 };
	Sonar::Vector2 m_MoveStartPos = { 0, 0 };
	int m_MoveHandleSize = 15;
};