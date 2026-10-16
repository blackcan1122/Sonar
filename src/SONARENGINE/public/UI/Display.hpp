#pragma once
#include "Base/BaseUI.h"
#include "Base/EventDispatcher.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/DisplayResizeData.hpp"
#include "Events/DisplayMoveData.hpp"
#include "raylib.h"

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
	virtual void MarkForDestruction() override;

	virtual void Initialize() override;

	void SetPosition(Vector2 NewPosition);
	void RenderToMainBuffer();

	virtual void ResizeDisplay(int NewWidth, int NewHeight);

	Vector2 GetSize() const { return { DestinationRect.width, DestinationRect.height }; }
	int GetWidth() const { return static_cast<int>(DestinationRect.width); }
	int GetHeight() const { return static_cast<int>(DestinationRect.height); }
	Rectangle GetDisplayedRect() const;

	void SetResizable(bool bCanResize) { bIsResizable = bCanResize; }
	bool IsResizable() const { return bIsResizable; }

	void SetMovable(bool bCanMove) { bIsMovable = bCanMove; }
	bool IsMovable() const { return bIsMovable; }
	bool IsManipulatingDisplay() const { return bIsResizing || bIsMoving; }

	void SetMinSize(int MinWidth, int MinHeight) { m_MinWidth = MinWidth; m_MinHeight = MinHeight; }

	virtual void Tick(float DeltaTime) override;

	Rectangle GetDestinationRect() const { return DestinationRect; }

	EventDispatcher OnResize;
	EventDispatcher OnMove;
	void SetDisplayPreviewRect(const Rectangle& PreviewRect);
	void ClearDisplayPreviewRect();

protected:
	mutable std::mutex m_ResizeMutex;

	void HandleResizeInteraction();
	void HandleMoveInteraction();
	void DrawResizeHandle();
	void DrawMoveHandle();
	Rectangle GetResizeHandleRect() const;
	Rectangle GetMoveHandleRect() const;

	RenderTexture2D ActiveRenderTarget;

	Rectangle SourceRect;
	Rectangle DestinationRect;

	std::shared_ptr<AllPurposeEvent> ResizeEvent;
	std::shared_ptr<AllPurposeEvent> ResizeEventComplete;

	std::shared_ptr<AllPurposeEvent> MoveEvent;
	std::shared_ptr<AllPurposeEvent> MoveEventComplete;

	std::shared_ptr<DisplayResizeData> ResizeData;
	std::shared_ptr<DisplayMoveData> MoveData;

	bool bIsResizable = true;
	bool bIsResizing = false;
	bool bIsResizeFinished = true;
	Vector2 m_ResizeStartMousePos = { 0, 0 };
	Vector2 m_ResizeStartSize = { 0, 0 };
	int m_ResizeHandleSize = 15;
	int m_MinWidth = 100;
	int m_MinHeight = 100;
	std::optional<Rectangle> m_DisplayPreviewRect;

	bool bIsMovable = true;
	bool bIsMoving = false;
	Vector2 m_MoveStartMousePos = { 0, 0 };
	Vector2 m_MoveStartPos = { 0, 0 };
	int m_MoveHandleSize = 15;
};