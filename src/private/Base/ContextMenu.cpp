#include "Base/ContextMenu.hpp"

void ContextMenu::Tick(float DeltaTime)
{
	if (IsConstructed)
	{
		ElapsedLifetime += DeltaTime;
		Draw();
	}
}

void ContextMenu::OnConstruct(Vector2 Position)
{
	CalculateSize();
	MousePosWhenConstructed = Position;
	Window.x = Position.x;
	Window.y = Position.y;

	float LastYPos = Window.y;

	for (int i = 0; i < MenuEntries.size(); i++)
	{
		Rectangle TempRec;

		TempRec = { Window.x, LastYPos + PaddingY, Window.width, (float)MenuEntries[i].FontSize };

		LastYPos = TempRec.y + TempRec.height + PaddingY;
		MenuEntries[i].ContextMenuEntryRec = TempRec;

	}

	ElapsedLifetime = 0.f;
	IsConstructed = true;
}

void ContextMenu::OnDelete()
{
	IsConstructed = false;
}

void ContextMenu::OnMouseButtonPressed(MouseButton Key, Vector2 MousePos)
{
	if (!IsConstructed)
	{
		return;
	}

	if (ElapsedLifetime >= CloseDelaySecs)
	{
		if (!CheckCollisionPointRec(MousePos, Window))
		{
			OnDelete();
			return;
		}
	}
	
	if (Key == MouseButton::MOUSE_BUTTON_LEFT)
	for (auto& Entry : MenuEntries)
	{		
		if (CheckCollisionPointRec(MousePos, Entry.ContextMenuEntryRec))
		{
			Entry.OnClick();
			//OnDelete();
			//return;
		}
	}

}

bool ContextMenu::GetIsConstructed() const
{
	return IsConstructed;
}

void ContextMenu::AddMenuEntry(ContextMenuEntry Entry)
{
	MenuEntries.push_back(Entry);
}

void ContextMenu::CalculateSize()
{
	int MaxLength = 0;
	int Height = 0;
	int Count = 1;
	for (auto& Entry : MenuEntries)
	{
		Entry.Construct();
		if (Entry.MeasuredText > MaxLength)
		{
			MaxLength = Entry.MeasuredText;
		}

		Height += Entry.FontSize + (PaddingY * 2);
	}

	Window.width = MaxLength;
	Window.height = Height;
}

void ContextMenu::Draw() const
{
	DrawRectangle(Window.x, Window.y, Window.width, Window.height, BackgroundColor);

	for (auto& Entry : MenuEntries)
	{
		if (CheckCollisionPointRec(GetMousePosition(), Entry.ContextMenuEntryRec) == false)
		{
			DrawRectangle(Entry.ContextMenuEntryRec.x, Entry.ContextMenuEntryRec.y, Entry.ContextMenuEntryRec.width, Entry.ContextMenuEntryRec.height, BLANK);
		}
		else
		{
			DrawRectangle(Entry.ContextMenuEntryRec.x, Entry.ContextMenuEntryRec.y, Entry.ContextMenuEntryRec.width, Entry.ContextMenuEntryRec.height, BLACK);
		}

		DrawText(Entry.GetDisplayName().c_str(), Entry.ContextMenuEntryRec.x, Entry.ContextMenuEntryRec.y, Entry.FontSize, GREEN);
	}
}

