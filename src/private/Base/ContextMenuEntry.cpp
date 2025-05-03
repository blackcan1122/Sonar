#include "Base/ContextMenuEntry.hpp"

void ContextMenuEntry::Construct()
{
	if (GetDisplayName() == "Unit")
	{
		LOG_ERROR(l_DEFAULT, TEXT("Missing Display Name for Context Menu. Skipping Construct"));
		return;
	}
	MeasuredText =  MeasureText(GetDisplayName().c_str(), FontSize);
}

void ContextMenuEntry::OnClick()
{
	OnClickCallback();
}

void ContextMenuEntry::SetCallback(void(*Function)(void))
{
	OnClickCallback = Function;
}
