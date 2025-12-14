#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"

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
	
	void SetMinSize(int MinWidth, int MinHeight) { m_MinWidth = MinWidth; m_MinHeight = MinHeight; }

	virtual void Tick(float DeltaTime) override;

protected:
	// Called during Tick to handle resize corner interaction
	void HandleResizeInteraction();
	
	// Draws the resize corner handle
	void DrawResizeHandle();
	
	// Get the resize handle rectangle
	Rectangle GetResizeHandleRect() const;

	RenderTexture2D ActiveRenderTarget;

	Rectangle SourceRect;
	Rectangle DestinationRect;
	
	// Resize handle properties
	bool bIsResizable = true;
	bool bIsResizing = false;
	Vector2 m_ResizeStartMousePos = { 0, 0 };
	Vector2 m_ResizeStartSize = { 0, 0 };
	int m_ResizeHandleSize = 15;
	int m_MinWidth = 100;
	int m_MinHeight = 100;

};