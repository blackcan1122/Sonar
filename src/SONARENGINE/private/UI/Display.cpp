#include "Base/Math.hpp"
#include "Rendering/GraphicSubSystem.hpp"
#include "Controls/ControlSubSystem.hpp"
#include "Base/GameInstance.h"
#include "UI/Display.hpp"
#include "Events/AllPurposeEvent.h"

Display::Display(int Width, int Height)
{
	ActiveRenderTarget = GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().LoadRenderTexture(Width, Height);
	SourceRect = { 0, 0, (float)Width, -(float)Height };
	DestinationRect = { 0, 0, (float)Width, (float)Height };
	m_TickGroup.SetTickGroup(ETickGroup::PostTick);
}

Display::~Display()
{
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().UnloadRenderTexture(ActiveRenderTarget);
}

void Display::Draw()
{
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().BeginTextureMode(ActiveRenderTarget);
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().ClearBackground(Sonar::BLACK);
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().EndTextureMode();
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

void Display::SetPosition(Sonar::Vector2 NewPosition)
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
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().DrawTexturePro(ActiveRenderTarget.texture, SourceRect, GetDisplayedRect(), { 0, 0 }, 0, Sonar::WHITE);
	DrawResizeHandle();
	DrawMoveHandle();
}

Sonar::Rectangle Display::GetDisplayedRect() const
{
	if (m_DisplayPreviewRect.has_value())
	{
		return *m_DisplayPreviewRect;
	}

	return DestinationRect;
}

void Display::SetDisplayPreviewRect(const Sonar::Rectangle& PreviewRect)
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

	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().UnloadRenderTexture(ActiveRenderTarget);
	ActiveRenderTarget = GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().LoadRenderTexture(NewWidth, NewHeight);

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

	Sonar::Vector2 mousePos = GameInstance::GetRuntimeContext().GetSubSystem<ControlSubSystem>().GetMousePosition();
	Sonar::Rectangle handleRect = GetResizeHandleRect();
	bool isOverHandle = Sonar::CheckCollisionPointRec(mousePos, handleRect);

	if (isOverHandle && GameInstance::GetRuntimeContext().GetSubSystem<ControlSubSystem>().IsMouseButtonPressed(Sonar::MOUSE_BUTTON_LEFT))
	{
		bIsResizing = true;
		m_ResizeStartMousePos = mousePos;
		m_ResizeStartSize = { DestinationRect.width, DestinationRect.height };
	}

	if (bIsResizing)
	{
		if (GameInstance::GetRuntimeContext().GetSubSystem<ControlSubSystem>().IsMouseButtonDown(Sonar::MOUSE_BUTTON_LEFT))
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
			const Sonar::Rectangle DisplayedRect = GetDisplayedRect();
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

	Sonar::Rectangle handleRect = GetResizeHandleRect();
	Sonar::Vector2 bottomRight = { handleRect.x + handleRect.width, handleRect.y + handleRect.height };
	Sonar::Color gripColor = bIsResizing ? Sonar::YELLOW : Sonar::GRAY;

	for (int i = 0; i < 3; i++)
	{
		float offset = (float)(i * 4 + 3);
		GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().DrawLine(
			(int)(bottomRight.x - offset), (int)bottomRight.y,
			(int)bottomRight.x, (int)(bottomRight.y - offset),
			gripColor
		);
	}
}

Sonar::Rectangle Display::GetResizeHandleRect() const
{
	const Sonar::Rectangle DisplayedRect = GetDisplayedRect();
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

	Sonar::Vector2 MousePos = GameInstance::GetRuntimeContext().GetSubSystem<ControlSubSystem>().GetMousePosition();
	Sonar::Rectangle HandleRect = GetMoveHandleRect();
	bool IsOverHandle = Sonar::CheckCollisionPointRec(MousePos, HandleRect);

	if (IsOverHandle && GameInstance::GetRuntimeContext().GetSubSystem<ControlSubSystem>().IsMouseButtonPressed(Sonar::MOUSE_BUTTON_LEFT))
	{
		bIsMoving = true;
		m_MoveStartMousePos = MousePos;
		m_MoveStartPos = { DestinationRect.x, DestinationRect.y };
	}

	if (bIsMoving)
	{
		if (GameInstance::GetRuntimeContext().GetSubSystem<ControlSubSystem>().IsMouseButtonDown(Sonar::MOUSE_BUTTON_LEFT))
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

	Sonar::Rectangle HandleRect = GetMoveHandleRect();
	Sonar::Color GripColor = bIsMoving ? Sonar::YELLOW : Sonar::GRAY;

	float CenterX = HandleRect.x + HandleRect.width / 2;
	float CenterY = HandleRect.y + HandleRect.height / 2;
	float Spacing = 3.0f;

	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().DrawCircle((int)CenterX, (int)(CenterY - Spacing), 2, GripColor);
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().DrawCircle((int)CenterX, (int)(CenterY + Spacing), 2, GripColor);
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().DrawCircle((int)(CenterX - Spacing), (int)CenterY, 2, GripColor);
	GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().DrawCircle((int)(CenterX + Spacing), (int)CenterY, 2, GripColor);
}

Sonar::Rectangle Display::GetMoveHandleRect() const
{
	const Sonar::Rectangle DisplayedRect = GetDisplayedRect();
	return {
		DisplayedRect.x,
		DisplayedRect.y,
		(float)m_MoveHandleSize,
		(float)m_MoveHandleSize
	};
}
void Display::BeginDisplayDraw() { GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().BeginTextureMode(ActiveRenderTarget); }
void Display::EndDisplayDraw() { GameInstance::GetRuntimeContext().GetSubSystem<GraphicSubSystem>().EndTextureMode(); }
