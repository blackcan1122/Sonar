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
	CalculateSize();
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

void ContextMenu::CalculateSize()
{
	int MaxLength = 0;
	int Height = 0;
	for (auto& const Entry : MenuEntries)
	{
		Entry.Construct();
		if (Entry.MeasuredText > MaxLength)
		{
			MaxLength = Entry.MeasuredText;
		}

		Height += Entry.FontSize + 2;
	}

	Window.width = MaxLength;
	Window.height = Height;
}

void ContextMenu::Draw() const
{
	DrawRectangle(Window.x, Window.y, Window.width, Window.height, BackgroundColor);

	int PaddingY = 0;
	for (auto const Entry : MenuEntries)
	{
		DrawText(Entry.GetDisplayName().c_str(), Window.x, Window.y + PaddingY, Entry.FontSize, GREEN);
		PaddingY += Entry.FontSize + 2;
	}
}

