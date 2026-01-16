#pragma once
#include "Base/Core.h"
#include "Base/BaseUI.h"
#include "Base/Texture2DWrap.hpp"

class UIEvent;
class EventDispatcher;

class Button : public BaseUI
{
	AUTOBODY(Button, BaseUI)

public:
	Button() = default;
	Button(int X, int Y, int Width, int Height, std::string InitialText, Color BackgroundColor);
	~Button() = default;

	virtual Button& Construct(Rectangle Dim, std::string InitialText, Color BackgroundColor);
	virtual Button& Construct(Rectangle Dim, std::string InitialText, Color BackgroundColor, bool Rounded, float Roundness);
	virtual Button& Construct(int X, int Y, int Width, int Height, std::string InitialText, Color BackgroundColor);
	virtual Button& Construct(int X, int Y, int Width, int Height, std::string InitialText, Color BackgroundColor, bool Rounded, float Roundness);
	
	virtual Button& SetEventDispatcher(std::shared_ptr<EventDispatcher> UsedDispatcher);
	virtual Button& SetEventDispatcher(std::weak_ptr<EventDispatcher> UsedDispatcher);


	virtual Button& SetEventPayload(std::string GameModeName);

	virtual Button& UpdateTextPosition(Vector2 NewPos);
	virtual Button& CenterText();
	virtual Button& UpdateTextPosition(int X, int Y);
	virtual Button& UpdateButtonPosition(Vector2 NewPos);
	virtual Button& UpdateButtonPosition(int X, int Y);
	virtual Button& UpdateColor(Color NewColor);
	virtual Button& UpdateText(std::string NewText);
	virtual Button& UpdateTextColor(Color NewTextColor);
	virtual Button& UpdateFontSize(int NewFontSize);
	virtual Button& SetStickyPosition(Vector2 StickyPos);
	virtual Button& SetStickyPosition(int X, int Y);

	virtual Button& CalculateRelativePosition();

	virtual Button& OnHover(std::function<void(Button* ButtonClass)> callback);

	virtual Button& SetTexture(SharedTexture2D Texture);
	virtual Button& SetNPatchInfo(NPatchInfo NPatchInfo);
	virtual Button& UseNPatchFeature(bool bUseNpatch);
	virtual Button& UseTexture(bool bUseTexture);

	// TODO: Refactor this out, this is just a workaround, as the Button Class right now doesn't really save its own State
	// or atleast make it optional
	virtual Button& OnHoverLeave(std::function<void(Button* ButtonClass)> callback); 

	virtual std::string GetEventPayload();
	
	virtual void Tick(float DeltaTime) override;
private:

	virtual std::shared_ptr<UIEvent> OnClickEvent();
	virtual bool IsClicked(Vector2 MousePosition, bool IsHovered);

	std::shared_ptr<EventDispatcher> UsedEventDispatcher;
	std::function<void(Button* ButtonClass)> HoverCallback;
	std::function<void(Button* ButtonClass)> HoverEndCallback;

	std::string m_Payload;
	std::string m_Text;
	Vector2 TextPosition;
	int FontSize = 14;
	bool m_IsRounded;
	bool m_WasLastHovered = false;
	float m_Roundness;
	Rectangle ButtonDim;
	Color m_BackgroundColor;
	Color m_TextColor = BLACK;

	Vector2 m_StickyPosition = { -1, -1 };
	Vector2 m_OffsetPosition = { -1, -1 };

	NPatchInfo m_NpatchTextureInfo;
	SharedTexture2D m_Texture;
	bool m_UseNpatch = false;
	bool m_UseTexture = false;

};