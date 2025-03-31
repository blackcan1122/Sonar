#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"

DECLARE_CLASS(Display, BaseUI)

public:
	Display(int Width, int Height);
	virtual ~Display();

	virtual void Update(float Deltatime);
	virtual void Draw();

	void SetPosition(Vector2 NewPosition);
	void RenderToMainBuffer();

protected:

	RenderTexture2D ActiveRenderTarget;

	Rectangle SourceRect;
	Rectangle DestinationRect;

END_CLASS