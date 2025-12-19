#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"

#include <mutex>

class GridLayoutManager;

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
	
	// Grid Layout Support
	void SetGridLayoutManager(GridLayoutManager* manager) { m_GridLayoutManager = manager; }
	GridLayoutManager* GetGridLayoutManager() const { return m_GridLayoutManager; }
	
	// Enable/disable grid snapping (when disabled, uses free positioning)
	void SetUseGridLayout(bool useGrid) { m_UseGridLayout = useGrid; }
	bool UsesGridLayout() const { return m_UseGridLayout; }
	
	// Get the current destination rect (for external layout managers)
	Rectangle GetDestinationRect() const { return DestinationRect; }

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
	
	// Grid layout support
	GridLayoutManager* m_GridLayoutManager = nullptr;
	bool m_UseGridLayout = true;
	GridCell m_PreviewCell;  // Used during drag to show where display will snap

};