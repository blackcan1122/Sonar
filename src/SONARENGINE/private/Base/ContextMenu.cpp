#include "Base/ContextMenu.hpp"

void ContextMenu::Tick(float DeltaTime)
{
	if (IsConstructed)
	{
		ElapsedLifetime += DeltaTime;
		Draw();
	}
}

void ContextMenu::OnConstruct(Sonar::Vector2 Position)
{
	CalculateSize();
	MousePosWhenConstructed = Position;
	Window.x = Position.x;
	Window.y = Position.y;

	float LastYPos = Window.y;

	for (int i = 0; i < MenuEntries.size(); i++)
	{
		Sonar::Rectangle TempRec { Window.x + 10, LastYPos + PaddingY, Window.width, (float)MenuEntries[i].FontSize };

		LastYPos = TempRec.y + TempRec.height + PaddingY;

		MenuEntries[i].ContextMenuEntryRec = TempRec;

	}

	ElapsedLifetime = 0.f;
	IsConstructed = true;
}

void ContextMenu::OnDelete()
{
	auto it = OnCloseCallbacks.begin();
	while (it != OnCloseCallbacks.end())
	{
		if (*it)
		{
			(*it)();
			++it;
		}
		else
		{
			it = OnCloseCallbacks.erase(it);
		}
	}

	IsConstructed = false;
}

void ContextMenu::OnMouseButtonPressed(Sonar::MouseButton Key, Sonar::Vector2 MousePos)
{
	if (!IsConstructed)
	{
		return;
	}

	if (ElapsedLifetime >= CloseDelaySecs)
	{
		if (!Sonar::CheckCollisionPointRec(MousePos, Window))
		{
			OnDelete();
			return;
		}
	}

	if (Key == Sonar::MouseButton::MOUSE_BUTTON_LEFT)
	for (auto& Entry : MenuEntries)
	{		
		if (Sonar::CheckCollisionPointRec(MousePos, Entry.ContextMenuEntryRec))
		{
			Entry.OnClick();
			if (Entry.CloseOnClick)
			{
				OnDelete();
			}
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

void ContextMenu::ClearMenuEntries()
{
	MenuEntries.clear();
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

	Window.width = static_cast<float>(MaxLength);
	Window.height = static_cast<float>(Height);
}

void ContextMenu::Draw() const
{
	Sonar::DrawRectangle(Window.x, Window.y, Window.width + 20, Window.height, BackgroundColor);

	for (auto& Entry : MenuEntries)
	{
		if (Sonar::CheckCollisionPointRec(Sonar::GetMousePosition(), Entry.ContextMenuEntryRec) == false)
		{
			Sonar::DrawRectangle(static_cast<int>(Entry.ContextMenuEntryRec.x), static_cast<int>(Entry.ContextMenuEntryRec.y), Entry.ContextMenuEntryRec.width, Entry.ContextMenuEntryRec.height, Sonar::BLANK);
		}
		else
		{
			Sonar::DrawRectangle(static_cast<int>(Entry.ContextMenuEntryRec.x), Entry.ContextMenuEntryRec.y, Entry.ContextMenuEntryRec.width, Entry.ContextMenuEntryRec.height, Sonar::BLACK);
		}

		Sonar::DrawText(Entry.GetDisplayName().c_str(), Entry.ContextMenuEntryRec.x, Entry.ContextMenuEntryRec.y, Entry.FontSize, Sonar::GREEN);
	}
}
