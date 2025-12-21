#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/EventDispatcher.hpp"
#include <mutex>

#include "Events/DisplayResizeData.hpp"
#include "Events/DisplayMoveData.hpp"

/**
 * @struct GridCell
 * @brief Represents a cell assignment for a Display in the grid.
 */
struct GridCell
{
	int row = 0;         // Starting row (0-indexed)
	int column = 0;      // Starting column (0-indexed)
	int rowSpan = 1;     // Number of rows this display occupies
	int colSpan = 1;     // Number of columns this display occupies
	
	bool operator==(const GridCell& other) const
	{
		return row == other.row && column == other.column && 
		       rowSpan == other.rowSpan && colSpan == other.colSpan;
	}
};

class Display : public BaseUI
{
	AUTOBODY(Display, BaseUI)
public:
	Display(int Width, int Height);
	virtual ~Display();


	virtual void Draw();
	virtual void  MarkForDestruction() override;

	virtual void Initialize() override;

	void SetPosition(Vector2 NewPosition);
	void RenderToMainBuffer();
	

	virtual void ResizeDisplay(int NewWidth, int NewHeight);
	
	Vector2 GetSize() const { return { DestinationRect.width, DestinationRect.height }; }
	int GetWidth() const { return static_cast<int>(DestinationRect.width); }
	int GetHeight() const { return static_cast<int>(DestinationRect.height); }
	
	void SetResizable(bool bCanResize) { bIsResizable = bCanResize; }
	bool IsResizable() const { return bIsResizable; }
	
	// Enable/disable move handle
	void SetMovable(bool bCanMove) { bIsMovable = bCanMove; }
	bool IsMovable() const { return bIsMovable; }
	
	void SetMinSize(int MinWidth, int MinHeight) { m_MinWidth = MinWidth; m_MinHeight = MinHeight; }

	virtual void Tick(float DeltaTime) override;
	
	// Get the current destination rect (for external layout managers)
	Rectangle GetDestinationRect() const { return DestinationRect; }

	EventDispatcher OnResize;
	EventDispatcher OnMove;

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
	Vector2 m_ResizeStartMousePos = { 0, 0 };
	Vector2 m_ResizeStartSize = { 0, 0 };
	int m_ResizeHandleSize = 15;
	int m_MinWidth = 100;
	int m_MinHeight = 100;
	
	bool bIsMovable = true;
	bool bIsMoving = false;
	Vector2 m_MoveStartMousePos = { 0, 0 };
	Vector2 m_MoveStartPos = { 0, 0 };
	int m_MoveHandleSize = 15;

};