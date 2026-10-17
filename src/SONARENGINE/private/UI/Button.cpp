#include "UI/Button.h"
#include "Events/UIEvent.h"
#include "Base/EventDispatcher.hpp"

Button::Button(int X, int Y, int Width, int Height, std::string InitialText, Sonar::Color BackgroundColor)
{
	Construct(X, Y, Width, Height, InitialText, BackgroundColor);
}

Button& Button::Construct(Sonar::Rectangle Dim, std::string InitialText, Sonar::Color BackgroundColor)
{
	ButtonDim = Dim;
	m_Text = InitialText;
	m_BacgroundColor = BackgroundColor;
	return *this;
}

Button& Button::Construct(Sonar::Rectangle Dim, std::string InitialText, Sonar::Color BackgroundColor, bool Rounded, float Roundness)
{
	ButtonDim = Dim;
	m_Text = InitialText;
	m_BacgroundColor = BackgroundColor;
	m_IsRounded = Rounded;
	m_Roundness = Roundness;
	return *this;
}

Button& Button::Construct(int X, int Y, int Width, int Height, std::string InitialText, Sonar::Color BackgroundColor)
{
	Sonar::Rectangle TempRec = { X, Y, Width, Height };
	return Construct(TempRec, InitialText, BackgroundColor);
}

Button& Button::Construct(int X, int Y, int Width, int Height, std::string InitialText, Sonar::Color BackgroundColor, bool Rounded, float Roundness)
{
	Sonar::Rectangle TempRec = { X, Y, Width, Height };
	return Construct(TempRec, InitialText, BackgroundColor, Rounded, Roundness);
}

Button& Button::SetEventDispatcher(std::shared_ptr<EventDispatcher> UsedDispatcher)
{
	UsedEventDispatcher = UsedDispatcher;
	return *this;
}

Button& Button::SetEventDispatcher(std::weak_ptr<EventDispatcher> UsedDispatcher)
{
	if (auto DispatcherPtr = UsedDispatcher.lock())
	{
		UsedEventDispatcher = DispatcherPtr;
	}
	return *this;
}

Button& Button::SetEventPayload(std::string Payload)
{
	m_Payload = Payload;
	return *this;
}

Button& Button::UpdateTextPosition(Sonar::Vector2 NewPos)
{
	TextPosition = NewPos;
	return *this;
}

Button& Button::CenterText()
{
	int LineCount = 1;
	size_t pos = 0;

	while ((pos = m_Text.find('\n', pos)) != std::string::npos)
	{
		LineCount++;
		pos++;
	}

	int textWidth = Sonar::MeasureText(m_Text.c_str(), FontSize);
	int LineHeight = FontSize * LineCount;

	float XPosition = (ButtonDim.width - textWidth) / 2.0f;
	float YPosition = (ButtonDim.height - LineHeight) / 2.0f;

	TextPosition.x = XPosition;
	TextPosition.y = YPosition;

	return *this;
}

Button& Button::UpdateTextPosition(int X, int Y)
{
	TextPosition.x = X;
	TextPosition.y = Y;
	return *this;
}

Button& Button::UpdateButtonPosition(Sonar::Vector2 NewPos)
{
	ButtonDim.x = NewPos.x;
	ButtonDim.y = NewPos.y;
	return *this;
}

Button& Button::UpdateButtonPosition(int X, int Y)
{
	ButtonDim.x = X;
	ButtonDim.y = Y;
	return *this;
}

Button& Button::UpdateColor(Sonar::Color NewColor)
{
	m_BacgroundColor = NewColor;
	return *this;
}

Button& Button::UpdateText(std::string NewText)
{
	m_Text = NewText;
	return *this;
}

Button& Button::UpdateTextColor(Sonar::Color NewTextColor)
{
	m_TextColor = NewTextColor;
	return *this;
}

Button& Button::UpdateFontSize(int NewFontSize)
{
	FontSize = NewFontSize;
	return *this;
}

Button& Button::SetStickyPosition(Sonar::Vector2 StickyPos)
{
	const int ScreenWidth = Sonar::GetScreenWidth();
	const int ScreenHeight = Sonar::GetScreenHeight();

	m_StickyPosition = StickyPos;
	UpdateButtonPosition(StickyPos);
	m_OffsetPosition = { StickyPos.x - ScreenWidth, StickyPos.y - ScreenHeight };

	return *this;
}

Button& Button::SetStickyPosition(int X, int Y)
{
	SetStickyPosition({ static_cast<float>(X), static_cast<float>(Y) });

	return *this;
}

Button& Button::CalculateRelativePosition()
{
	Sonar::Vector2 CurrentPos = { static_cast<float>(Sonar::GetScreenWidth()) + m_OffsetPosition.x, static_cast<float>(Sonar::GetScreenHeight()) + m_OffsetPosition.y };
	UpdateButtonPosition(CurrentPos);

	return *this;
}

Button& Button::OnHover(std::function<void(Button* ButtonClass)> callback)
{
	HoverCallback = callback;
	return *this;
}

Button& Button::SetTexture(SharedTexture2D Texture)
{
	m_Texture = Texture;
	return *this;
}

Button& Button::SetNPatchInfo(Sonar::NPatchInfo PatchInfo)
{
	m_NpatchTextureInfo = PatchInfo;
	return *this;
}

Button& Button::UseNPatchFeature(bool bUseNpatch)
{
	m_UseNpatch = bUseNpatch;
	return *this;
}

Button& Button::UseTexture(bool bUseTexture)
{
	m_UseTexture = bUseTexture;
	return *this;
}

Button& Button::OnHoverLeave(std::function<void(Button* ButtonClass)> callback)
{
	HoverEndCallback = callback;
	return *this;
}

std::string Button::GetEventPayload()
{
	return m_Payload;
}

std::shared_ptr<UIEvent> Button::OnClickEvent()
{
	std::shared_ptr<UIEvent> EventUI = std::make_shared<UIEvent>();
	EventUI->ClickedUIElement = this;
	EventUI->Payload = m_Payload;
	return EventUI;
}

bool Button::IsClicked(Sonar::Vector2 MousePosition, bool IsHovered)
{
	return Sonar::IsMouseButtonPressed(0) && IsHovered;
}

void Button::Tick(float DeltaTime)
{
	bool isHovered = (Sonar::CheckCollisionPointRec(Sonar::GetMousePosition(), ButtonDim) && Sonar::IsCursorOnScreen());

	if (isHovered && m_WasLastHovered == false && HoverCallback)
	{
		HoverCallback(this);
	}
	else if (!isHovered && m_WasLastHovered == true && HoverEndCallback)
	{
		HoverEndCallback(this);
	}

	m_WasLastHovered = isHovered;

	if (IsClicked(Sonar::GetMousePosition(), isHovered))
	{
		if (UsedEventDispatcher)
		{
			std::cout << "Dispatching Event" << std::endl;
			UsedEventDispatcher->Dispatch(OnClickEvent());
		}
	}

	if (!m_UseTexture)
	{
		if (m_IsRounded)
		{
			Sonar::DrawRectangleRounded(ButtonDim, m_Roundness, 32, m_BacgroundColor);
		}
		else
		{
			Sonar::DrawRectangle(ButtonDim.x, ButtonDim.y, ButtonDim.width, ButtonDim.height, m_BacgroundColor);
		}
	}
	else
	{
		if (m_UseNpatch)
		{
			Sonar::DrawTextureNPatch(m_Texture, m_NpatchTextureInfo, ButtonDim, { 0, 0 }, 0, Sonar::WHITE);
		}
		else
		{
			Sonar::DrawTexture(m_Texture, ButtonDim.x, ButtonDim.y, Sonar::WHITE);
		}
	}

	Sonar::DrawText(m_Text.c_str(), ButtonDim.x + TextPosition.x, ButtonDim.y + TextPosition.y, FontSize, m_TextColor);
}