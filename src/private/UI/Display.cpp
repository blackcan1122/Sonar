#include "UI/Display.hpp"
#include "Events/AllPurposeEvent.h"


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

void Display::Initialize()
{
	ResizeEvent = std::make_shared<AllPurposeEvent>();
	ResizeEventComplete = std::make_shared<AllPurposeEvent>();

	MoveEvent = std::make_shared<AllPurposeEvent>();
	MoveEventComplete = std::make_shared<AllPurposeEvent>();

	ResizeData = std::make_shared<DisplayResizeData>();
	MoveData = std::make_shared<DisplayMoveData>();

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
	
	UnloadRenderTexture(ActiveRenderTarget);
	
	ActiveRenderTarget = LoadRenderTexture(NewWidth, NewHeight);
	
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
			float deltaX = mousePos.x - m_ResizeStartMousePos.x;
			float deltaY = mousePos.y - m_ResizeStartMousePos.y;
			int OldWidth = static_cast<int>(DestinationRect.width);
			int OldHeight = static_cast<int>(DestinationRect.height);
			
			int NewWidth = static_cast<int>(m_ResizeStartSize.x + deltaX);
			int NewHeight = static_cast<int>(m_ResizeStartSize.y + deltaY);
			
			NewWidth = (NewWidth < m_MinWidth) ? m_MinWidth : NewWidth;
			NewHeight = (NewHeight < m_MinHeight) ? m_MinHeight : NewHeight;
			
			ResizeData->DisplayToResize = SoftObjectPath<Display>(this->m_Name);
			ResizeData->NewWidth = NewWidth;
			ResizeData->NewHeight = NewHeight;
			ResizeData->OldWidth = OldWidth;
			ResizeData->OldHeight = OldHeight;
			ResizeData->bIsResizeComplete = false;
			ResizeEvent->Payload = ResizeData;

			OnResize.Dispatch(ResizeEvent);
		}
		else
		{
			// Stop resizing when mouse released
			bIsResizing = false;
			
			// Dispatch resize complete event - GridLayoutManager will handle snapping
			ResizeData->DisplayToResize = SoftObjectPath<Display>(this->m_Name);
			ResizeData->NewWidth = static_cast<int>(DestinationRect.width);
			ResizeData->NewHeight = static_cast<int>(DestinationRect.height);
			ResizeData->OldWidth = static_cast<int>(m_ResizeStartSize.x);
			ResizeData->OldHeight = static_cast<int>(m_ResizeStartSize.y);
			ResizeData->bIsResizeComplete = true;
			ResizeEventComplete->Payload = ResizeData;

			OnResize.Dispatch(ResizeEventComplete);
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
	
	bool IsOverHandle = CheckCollisionPointRec(MousePos, HandleRect);
	
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
			
			// Dispatch move event - let listeners handle constraints and previews
			MoveData->DisplayToMove = SoftObjectPath<Display>(this->m_Name);
			MoveData->NewX = newX;
			MoveData->NewY = newY;
			MoveData->MouseX = MousePos.x;
			MoveData->MouseY = MousePos.y;
			MoveData->DisplayWidth = DestinationRect.width;
			MoveData->DisplayHeight = DestinationRect.height;
			MoveData->bIsMoveComplete = false;
			MoveEvent->Payload = MoveData;

			OnMove.Dispatch(MoveEvent);
		}
		else
		{
			// Stop moving when mouse released
			bIsMoving = false;
			
			// Dispatch move complete event
			MoveData->DisplayToMove = SoftObjectPath<Display>(this->m_Name);
			MoveData->NewX = DestinationRect.x;
			MoveData->NewY = DestinationRect.y;
			MoveData->MouseX = MousePos.x;
			MoveData->MouseY = MousePos.y;
			MoveData->DisplayWidth = DestinationRect.width;
			MoveData->DisplayHeight = DestinationRect.height;
			MoveData->bIsMoveComplete = true;
			MoveEventComplete->Payload = MoveData;

			OnMove.Dispatch(MoveEventComplete);
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
