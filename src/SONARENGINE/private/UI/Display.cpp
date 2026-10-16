#include "UI/Display.hpp"
#include "Events/AllPurposeEvent.h"

Display::Display(int Width, int Height)
{
	ActiveRenderTarget = LoadRenderTexture(Width, Height);
	SourceRect = { 0, 0, (float)Width, -(float)Height };
	DestinationRect = { 0, 0, (float)Width, (float)Height };
	m_TickGroup.SetTickGroup(ETickGroup::PostTick);
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
	if (m_DisplayPreviewRect.has_value())
	{
		m_DisplayPreviewRect->x = NewPosition.x;
		m_DisplayPreviewRect->y = NewPosition.y;
	}
}

void Display::RenderToMainBuffer()
{
	DrawTexturePro(ActiveRenderTarget.texture, SourceRect, GetDisplayedRect(), { 0, 0 }, 0, WHITE);
	DrawResizeHandle();
	DrawMoveHandle();
}

Rectangle Display::GetDisplayedRect() const
{
	if (m_DisplayPreviewRect.has_value())
	{
		return *m_DisplayPreviewRect;
	}

	return DestinationRect;
}

void Display::SetDisplayPreviewRect(const Rectangle& PreviewRect)
{
	m_DisplayPreviewRect = PreviewRect;
}

void Display::ClearDisplayPreviewRect()
{
	m_DisplayPreviewRect.reset();
}

void Display::ResizeDisplay(int NewWidth, int NewHeight)
{
	if (NewWidth <= 0 || NewHeight <= 0)
	{
		return;
	}

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
	ClearDisplayPreviewRect();
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
	bool isOverHandle = CheckCollisionPointRec(mousePos, handleRect);

	if (isOverHandle && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		bIsResizing = true;
		m_ResizeStartMousePos = mousePos;
		m_ResizeStartSize = { DestinationRect.width, DestinationRect.height };
	}

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

			SetDisplayPreviewRect({
				DestinationRect.x,
				DestinationRect.y,
				(float)NewWidth,
				(float)NewHeight
			});

			ResizeData->DisplayToResize = SoftObjectPath<Display>(this->m_Name);
			ResizeData->NewWidth = NewWidth;
			ResizeData->NewHeight = NewHeight;
			ResizeData->OldWidth = OldWidth;
			ResizeData->OldHeight = OldHeight;
			ResizeData->bIsResizeComplete = false;
			bIsResizeFinished = false;
			ResizeEvent->Payload = ResizeData;

			OnResize.Dispatch(ResizeEvent);
		}
		else
		{
			const Rectangle DisplayedRect = GetDisplayedRect();
			bIsResizing = false;

			ResizeData->DisplayToResize = SoftObjectPath<Display>(this->m_Name);
			ResizeData->NewWidth = static_cast<int>(DisplayedRect.width);
			ResizeData->NewHeight = static_cast<int>(DisplayedRect.height);
			ResizeData->OldWidth = static_cast<int>(m_ResizeStartSize.x);
			ResizeData->OldHeight = static_cast<int>(m_ResizeStartSize.y);
			ResizeData->bIsResizeComplete = true;
			bIsResizeFinished = true;
			ResizeEventComplete->Payload = ResizeData;

			OnResize.Dispatch(ResizeEventComplete);
			ClearDisplayPreviewRect();
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
	Vector2 bottomRight = { handleRect.x + handleRect.width, handleRect.y + handleRect.height };
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
	const Rectangle DisplayedRect = GetDisplayedRect();
	return {
		DisplayedRect.x + DisplayedRect.width - m_ResizeHandleSize,
		DisplayedRect.y + DisplayedRect.height - m_ResizeHandleSize,
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

	if (bIsMoving)
	{
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			float DeltaX = MousePos.x - m_MoveStartMousePos.x;
			float DeltaY = MousePos.y - m_MoveStartMousePos.y;

			float newX = m_MoveStartPos.x + DeltaX;
			float newY = m_MoveStartPos.y + DeltaY;

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
			bIsMoving = false;

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
	const Rectangle DisplayedRect = GetDisplayedRect();
	return {
		DisplayedRect.x,
		DisplayedRect.y,
		(float)m_MoveHandleSize,
		(float)m_MoveHandleSize
	};
}