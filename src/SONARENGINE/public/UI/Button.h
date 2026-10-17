#pragma once
#include "Base/BaseUI.h"
#include "Base/Texture2DWrap.hpp"
#include "Rendering/Graphics.hpp"

#include <functional>
#include <memory>
#include <string>

class UIEvent;
class EventDispatcher;

class SONARENGINE_API Button : public BaseUI
{
	AUTOBODY(Button, BaseUI)

public:
	Button() = default;
	Button(int X, int Y, int Width, int Height, std::string InitialText, Sonar::Color BackgroundColor);
	~Button() = default;

	virtual Button& Construct(Sonar::Rectangle Dim, std::string InitialText, Sonar::Color BackgroundColor);
	virtual Button& Construct(Sonar::Rectangle Dim, std::string InitialText, Sonar::Color BackgroundColor, bool Rounded, float Roundness);
	virtual Button& Construct(int X, int Y, int Width, int Height, std::string InitialText, Sonar::Color BackgroundColor);
	virtual Button& Construct(int X, int Y, int Width, int Height, std::string InitialText, Sonar::Color BackgroundColor, bool Rounded, float Roundness);

	virtual Button& SetEventDispatcher(std::shared_ptr<EventDispatcher> UsedDispatcher);
	virtual Button& SetEventDispatcher(std::weak_ptr<EventDispatcher> UsedDispatcher);

	virtual Button& SetEventPayload(std::string GameModeName);

	virtual Button& UpdateTextPosition(Sonar::Vector2 NewPos);
	virtual Button& CenterText();
	virtual Button& UpdateTextPosition(int X, int Y);
	virtual Button& UpdateButtonPosition(Sonar::Vector2 NewPos);
	virtual Button& UpdateButtonPosition(int X, int Y);
	virtual Button& UpdateColor(Sonar::Color NewColor);
	virtual Button& UpdateText(std::string NewText);
	virtual Button& UpdateTextColor(Sonar::Color NewTextColor);
	virtual Button& UpdateFontSize(int NewFontSize);
	virtual Button& SetStickyPosition(Sonar::Vector2 StickyPos);
	virtual Button& SetStickyPosition(int X, int Y);

	virtual Button& CalculateRelativePosition();

	virtual Button& OnHover(std::function<void(Button* ButtonClass)> callback);

	virtual Button& SetTexture(SharedTexture2D Texture);
	virtual Button& SetNPatchInfo(Sonar::NPatchInfo PatchInfo);
	virtual Button& UseNPatchFeature(bool bUseNpatch);
	virtual Button& UseTexture(bool bUseTexture);

	virtual Button& OnHoverLeave(std::function<void(Button* ButtonClass)> callback);

	virtual std::string GetEventPayload();

	virtual void Tick(float DeltaTime) override;
private:

	virtual std::shared_ptr<UIEvent> OnClickEvent();
	virtual bool IsClicked(Sonar::Vector2 MousePosition, bool IsHovered);

	std::shared_ptr<EventDispatcher> UsedEventDispatcher;
	std::function<void(Button* ButtonClass)> HoverCallback;
	std::function<void(Button* ButtonClass)> HoverEndCallback;

	std::string m_Payload;
	std::string m_Text;
	Sonar::Vector2 TextPosition;
	int FontSize = 14;
	bool m_IsRounded;
	bool m_WasLastHovered = false;
	float m_Roundness;
	Sonar::Rectangle ButtonDim;
	Sonar::Color m_BacgroundColor;
	Sonar::Color m_TextColor = Sonar::BLACK;

	Sonar::Vector2 m_StickyPosition = { -1, -1 };
	Sonar::Vector2 m_OffsetPosition = { -1, -1 };

	Sonar::NPatchInfo m_NpatchTextureInfo;
	SharedTexture2D m_Texture;
	bool m_UseNpatch = false;
	bool m_UseTexture = false;
};