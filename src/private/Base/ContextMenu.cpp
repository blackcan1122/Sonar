#include "Base/ContextMenu.hpp"

void ContextMenu::Tick(float DeltaTime)
{

}

void ContextMenu::TestTick()
{
	if (IsConstructed)
	{
		Draw();
	}
}

void ContextMenu::OnConstruct(Vector2 Position)
{
	Window.x = Position.x;
	Window.y = Position.y;
	IsConstructed = true;
}

void ContextMenu::OnDelete()
{
	IsConstructed = false;
}

void ContextMenu::AddMenuEntry(ContextMenuEntry Entry)
{
	MenuEntries.push_back(Entry);
}

void ContextMenu::SetSize(Vector2 Size)
{
	Window.width = Size.x;
	Window.height = Size.y;
}

void ContextMenu::Draw() const
{
	DrawRectangle(Window.x, Window.y, Window.width, Window.height, BackgroundColor);
}

