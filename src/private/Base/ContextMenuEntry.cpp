#include "Base/ContextMenuEntry.hpp"

void ContextMenuEntry::Construct()
{
	if (GetDisplayName() == "Unit")
	{
		LOG_ERROR(l_DEFAULT, TEXT("Missing Display Name for Context Menu. Should be Changed"));
	}
	MeasuredText =  MeasureText(GetDisplayName().c_str(), FontSize);
}

void ContextMenuEntry::OnClick()
{
	if (Callback != nullptr)
	{
		Callback(this);
	}
}

void ContextMenuEntry::SetCallback(std::function<void(ContextMenuEntry* Self)> NewCallback)
{
	Callback = NewCallback;
}
