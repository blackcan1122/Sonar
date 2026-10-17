#include "Base/ContextMenuEntry.hpp"
#include "Base/Logging.h"

void ContextMenuEntry::Construct()
{
	if (GetDisplayName() == "Unit")
	{
		LOG_ERROR(l_DEFAULT, SONAR_FMT("Missing Display Name for Context Menu. Should be Changed"));
	}
	MeasuredText =  Sonar::MeasureText(GetDisplayName().c_str(), FontSize);
}

void ContextMenuEntry::OnClick()
{
	if (Callback)
	{
		Callback(this);
	}
	else
	{
		LOG_WARN(SONAR_FMT("ContextMenuEntry::OnClick - No callback set for menu entry '{}'.", GetDisplayName()));
	}
}

void ContextMenuEntry::SetCallback(std::function<void(ContextMenuEntry* Self)> NewCallback)
{
	Callback = NewCallback;
}
