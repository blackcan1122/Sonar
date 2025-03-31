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

void Display::Update(float Deltatime)
{
	
}

void Display::Draw()
{
	BeginTextureMode(ActiveRenderTarget);
	ClearBackground(BLACK);
	EndTextureMode();
}

void Display::SetPosition(Vector2 NewPosition)
{
	DestinationRect.x = NewPosition.x;
	DestinationRect.y = NewPosition.y;
}

void Display::RenderToMainBuffer()
{
	DrawTexturePro(ActiveRenderTarget.texture, SourceRect, DestinationRect, { 0,0 }, 0, WHITE);
}
