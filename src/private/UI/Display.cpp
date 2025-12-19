#include "UI/Display.hpp"
#include "UI/GridLayoutManager.hpp"

Display::Display(int Width, int Height)
{
	ActiveRenderTarget = LoadRenderTexture(Width, Height);
	SourceRect = {0,0, (float)Width, -(float)Height};
	DestinationRect = {0,0, (float)Width, (float)Height};
}

Display::~Display()
{
	UnloadRenderTexture(ActiveRenderTarget);
}

void Display::Draw()
{
	BeginTextureMode(ActiveRenderTarget);
	ClearBackground(BLACK);
	EndTextureMode();
}

void Display::MarkForDestruction()
{
	Super::MarkForDestruction();
}

void Display::SetPosition(Vector2 NewPosition)
{
	DestinationRect.x = NewPosition.x;
	DestinationRect.y = NewPosition.y;
}

void Display::RenderToMainBuffer()
{
	DrawTexturePro(ActiveRenderTarget.texture, SourceRect, DestinationRect, { 0,0 }, 0, WHITE);
	DrawResizeHandle();
	DrawMoveHandle();
}

void Display::ResizeDisplay(int NewWidth, int NewHeight)
{
	if (NewWidth <= 0 || NewHeight <= 0)
	{
		return;
	}
	
	// Don't resize if same size
	if (NewWidth == static_cast<int>(DestinationRect.width) && 
	    NewHeight == static_cast<int>(DestinationRect.height))
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_ResizeMutex);
	
	// Unload old render texture
	UnloadRenderTexture(ActiveRenderTarget);
	
	// Create new render texture with new dimensions
	ActiveRenderTarget = LoadRenderTexture(NewWidth, NewHeight);
	
	// Update rectangles
	SourceRect = { 0, 0, (float)NewWidth, -(float)NewHeight };
	DestinationRect.width = (float)NewWidth;
	DestinationRect.height = (float)NewHeight;
}

void Display::Tick(float DeltaTime)
{
	HandleMoveInteraction();
	HandleResizeInteraction();
}

void Display::HandleResizeInteraction()
{
	if (!bIsResizable)
	{
		return;
	}
	
	Vector2 mousePos = GetMousePosition();
	Rectangle handleRect = GetResizeHandleRect();
	
	// Check if mouse is over resize handle
	bool isOverHandle = CheckCollisionPointRec(mousePos, handleRect);
	
	// Start resizing
	if (isOverHandle && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		bIsResizing = true;
		m_ResizeStartMousePos = mousePos;
		m_ResizeStartSize = { DestinationRect.width, DestinationRect.height };
	}
	
	// Continue resizing
	if (bIsResizing)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			// Calculate new size based on mouse delta
			float deltaX = mousePos.x - m_ResizeStartMousePos.x;
			float deltaY = mousePos.y - m_ResizeStartMousePos.y;
			
			int newWidth = static_cast<int>(m_ResizeStartSize.x + deltaX);
			int newHeight = static_cast<int>(m_ResizeStartSize.y + deltaY);
			
			// Clamp to minimum size
			newWidth = (newWidth < m_MinWidth) ? m_MinWidth : newWidth;
			newHeight = (newHeight < m_MinHeight) ? m_MinHeight : newHeight;
			
			// If using grid layout, clamp size to stay within grid bounds
			if (m_UseGridLayout && m_GridLayoutManager)
			{
				int gridWidth = m_GridLayoutManager->GetWindowWidth();
				int gridHeight = m_GridLayoutManager->GetWindowHeight();
				
				// Calculate maximum size that keeps display within grid
				int maxWidth = gridWidth - static_cast<int>(DestinationRect.x);
				int maxHeight = gridHeight - static_cast<int>(DestinationRect.y);
				
				newWidth = std::min(newWidth, maxWidth);
				newHeight = std::min(newHeight, maxHeight);
				
				// Calculate and show preview of which cells will be occupied
				int cellWidth = m_GridLayoutManager->GetCellWidth();
				int cellHeight = m_GridLayoutManager->GetCellHeight();
				
				if (cellWidth > 0 && cellHeight > 0)
				{
					const GridCell* currentCell = m_GridLayoutManager->GetDisplayCell(this);
					if (currentCell)
					{
						// Calculate how many cells this size would occupy
						int previewColSpan = std::max(1, static_cast<int>(std::round(newWidth / static_cast<float>(cellWidth))));
						int previewRowSpan = std::max(1, static_cast<int>(std::round(newHeight / static_cast<float>(cellHeight))));
						
						// Clamp span to not exceed grid bounds
						previewColSpan = std::min(previewColSpan, m_GridLayoutManager->GetColumns() - currentCell->column);
						previewRowSpan = std::min(previewRowSpan, m_GridLayoutManager->GetRows() - currentCell->row);
						
						GridCell previewCell = *currentCell;
						previewCell.colSpan = previewColSpan;
						previewCell.rowSpan = previewRowSpan;
						
						m_GridLayoutManager->SetDisplayResizing(this, true, previewCell);
					}
				}
			}
			
			// Only resize if size actually changed
			if (newWidth != GetWidth() || newHeight != GetHeight())
			{
				ResizeDisplay(newWidth, newHeight);
			}
		}
		else
		{
			// Stop resizing when mouse released
			bIsResizing = false;
			
			// Clear the resize preview
			if (m_UseGridLayout && m_GridLayoutManager)
			{
				m_GridLayoutManager->SetDisplayResizing(this, false, {});
			}
			
			// If using grid layout, calculate new span based on final size
			if (m_UseGridLayout && m_GridLayoutManager)
			{
				int cellWidth = m_GridLayoutManager->GetCellWidth();
				int cellHeight = m_GridLayoutManager->GetCellHeight();
				
				if (cellWidth > 0 && cellHeight > 0)
				{
					// Calculate how many cells this size would occupy
					int newColSpan = std::max(1, static_cast<int>(std::round(DestinationRect.width / static_cast<float>(cellWidth))));
					int newRowSpan = std::max(1, static_cast<int>(std::round(DestinationRect.height / static_cast<float>(cellHeight))));
					
					// Try to resize the span (this will fail if overlapping)
					m_GridLayoutManager->ResizeDisplaySpan(this, newRowSpan, newColSpan);
				}
			}
		}
	}
}

void Display::DrawResizeHandle()
{
	if (!bIsResizable)
	{
		return;
	}
	
	Rectangle handleRect = GetResizeHandleRect();
	
	// Draw corner triangle/grip
	Vector2 bottomRight = { handleRect.x + handleRect.width, handleRect.y + handleRect.height };

	
	// Draw diagonal grip lines
	Color gripColor = bIsResizing ? YELLOW : GRAY;
	
	for (int i = 0; i < 3; i++)
	{
		float offset = (float)(i * 4 + 3);
		DrawLine(
			(int)(bottomRight.x - offset), (int)bottomRight.y,
			(int)bottomRight.x, (int)(bottomRight.y - offset),
			gripColor
		);
	}
}

Rectangle Display::GetResizeHandleRect() const
{
	return {
		DestinationRect.x + DestinationRect.width - m_ResizeHandleSize,
		DestinationRect.y + DestinationRect.height - m_ResizeHandleSize,
		(float)m_ResizeHandleSize,
		(float)m_ResizeHandleSize
	};
}

void Display::HandleMoveInteraction()
{
	if (!bIsMovable)
	{
		return;
	}
	
	Vector2 MousePos = GetMousePosition();
	Rectangle HandleRect = GetMoveHandleRect();
	
	// Check if mouse is over move handle
	bool IsOverHandle = CheckCollisionPointRec(MousePos, HandleRect);
	
	// Start moving
	if (IsOverHandle && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		bIsMoving = true;
		m_MoveStartMousePos = MousePos;
		m_MoveStartPos = { DestinationRect.x, DestinationRect.y };
	}
	
	// Continue moving
	if (bIsMoving)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			// Calculate new position based on mouse delta
			float DeltaX = MousePos.x - m_MoveStartMousePos.x;
			float DeltaY = MousePos.y - m_MoveStartMousePos.y;
			
			float newX = m_MoveStartPos.x + DeltaX;
			float newY = m_MoveStartPos.y + DeltaY;
			
			// If using grid layout, clamp position within grid bounds
			if (m_UseGridLayout && m_GridLayoutManager)
			{
				int gridWidth = m_GridLayoutManager->GetWindowWidth();
				int gridHeight = m_GridLayoutManager->GetWindowHeight();
				
				// Clamp to keep display within grid bounds
				newX = std::max(0.0f, std::min(newX, static_cast<float>(gridWidth - DestinationRect.width)));
				newY = std::max(0.0f, std::min(newY, static_cast<float>(gridHeight - DestinationRect.height)));
				
				// Calculate preview cell and register for overlay drawing
				m_PreviewCell = m_GridLayoutManager->SnapToGrid(this, MousePos, true);
				m_GridLayoutManager->SetDisplayDragging(this, true, m_PreviewCell);
			}
			
			DestinationRect.x = newX;
			DestinationRect.y = newY;
		}
		else
		{
			// Stop moving when mouse released
			bIsMoving = false;
			
			// Snap to grid on release and unregister from dragging
			if (m_UseGridLayout && m_GridLayoutManager)
			{
				m_GridLayoutManager->SetDisplayDragging(this, false, m_PreviewCell);
				m_GridLayoutManager->MoveDisplayToCell(this, m_PreviewCell);
			}
		}
	}
}

void Display::DrawMoveHandle()
{
	if (!bIsMovable)
	{
		return;
	}
	
	Rectangle HandleRect = GetMoveHandleRect();
	
	Color GripColor = bIsMoving ? YELLOW : GRAY;
	
	float CenterX = HandleRect.x + HandleRect.width / 2;
	float CenterY = HandleRect.y + HandleRect.height / 2;
	float Spacing = 3.0f;
	
	DrawCircle((int)CenterX, (int)(CenterY - Spacing), 2, GripColor);
	DrawCircle((int)CenterX, (int)(CenterY + Spacing), 2, GripColor);
	DrawCircle((int)(CenterX - Spacing), (int)CenterY, 2, GripColor);
	DrawCircle((int)(CenterX + Spacing), (int)CenterY, 2, GripColor);
}

Rectangle Display::GetMoveHandleRect() const
{
	return {
		DestinationRect.x,
		DestinationRect.y,
		(float)m_MoveHandleSize,
		(float)m_MoveHandleSize
	};
}
