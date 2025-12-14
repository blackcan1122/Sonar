#include "UI/Display.hpp"

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
