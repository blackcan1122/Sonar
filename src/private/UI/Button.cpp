#include "UI/Button.h"
#include "Events/UIEvent.h"
#include "Base/EventDispatcher.hpp"

Button::Button(int X, int Y, int Width, int Height, std::string InitialText, Color BackgroundColor)
{
    Construct(X, Y, Width, Height, InitialText, BackgroundColor);
}

Button& Button::Construct(Rectangle Dim, std::string InitialText, Color BackgroundColor)
{
    ButtonDim = Dim;
    m_Text = InitialText;
    m_BackgroundColor = BackgroundColor;
    return *this;
}

Button& Button::Construct(Rectangle Dim, std::string InitialText, Color BackgroundColor, bool Rounded, float Roundness)
{
    ButtonDim = Dim;
    m_Text = InitialText;
    m_BackgroundColor = BackgroundColor;
    m_IsRounded = Rounded;
    m_Roundness = Roundness;
    return *this;
}

Button& Button::Construct(int X, int Y, int Width, int Height, std::string InitialText, Color BackgroundColor)
{
    Rectangle TempRec = { X, Y, Width, Height };
    return Construct(TempRec, InitialText, BackgroundColor);
}

Button& Button::Construct(int X, int Y, int Width, int Height, std::string InitialText, Color BackgroundColor, bool Rounded, float Roundness)
{
    Rectangle TempRec = { X, Y, Width, Height };
    return Construct(TempRec, InitialText, BackgroundColor, Rounded, Roundness);
}   

Button& Button::SetEventDispatcher(std::shared_ptr<EventDispatcher> UsedDispatcher)
{
    UsedEventDispatcher = UsedDispatcher;
    return *this;
}

Button& Button::SetEventPayload(std::string Payload)
{
    m_Payload = Payload;
    return *this;
}

Button& Button::UpdateTextPosition(Vector2 NewPos)
{
    TextPosition = NewPos;
    return *this;
}

Button& Button::CenterText()
{
    int LineCount = 1; // Starting with 1 since we always have atleast 1 line
    size_t pos = 0;

    while ((pos = m_Text.find('\n', pos)) != std::string::npos)
    {
        LineCount++;
        pos++;
    }

    // Measure the width of the text
    int textWidth = MeasureText(m_Text.c_str(), FontSize);
    int LineHeight = FontSize * LineCount;

    // Assuming ButtonDim.x is the button's width, center the text in local space:
    float XPosition = (ButtonDim.width - textWidth) / 2.0f;
    float YPosition = (ButtonDim.height - LineHeight) / 2.0f;

    // Update the local X offset; Y remains unchanged
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

Button& Button::UpdateButtonPosition(Vector2 NewPos)
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

Button& Button::UpdateColor(Color NewColor)
{
    m_BackgroundColor = NewColor;
    return *this;
}

Button& Button::UpdateText(std::string NewText)
{
    m_Text = NewText;
    return *this;
}

Button& Button::UpdateTextColor(Color NewTextColor)
{
    m_TextColor = NewTextColor;
    return *this;
}

Button& Button::UpdateFontSize(int NewFontSize)
{
    FontSize = NewFontSize;
    return *this;
}


Button& Button::OnHover(std::function<void(Button* ButtonClass)> callback)
{
    HoverCallback = callback;
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

void Button::Update()
{
    bool isHovered = (CheckCollisionPointRec(GetMousePosition(), ButtonDim) && IsCursorOnScreen());

    if (isHovered && m_WasLastHovered == false && HoverCallback)
    {
        HoverCallback(this);
    }

    else if (!isHovered && m_WasLastHovered == true && HoverEndCallback)
    {
        HoverEndCallback(this);
    }

    m_WasLastHovered = isHovered;

    if (IsClicked(GetMousePosition(), isHovered))
    {
        if (UsedEventDispatcher)
        {
            std::cout << "Dispatching Event" << std::endl;
            UsedEventDispatcher->Dispatch(OnClickEvent());
        }

    }
    if (m_IsRounded)
    {
        DrawRectangleRounded(ButtonDim, m_Roundness, 32, m_BackgroundColor);
    }
    else
    {
        DrawRectangle(ButtonDim.x, ButtonDim.y, ButtonDim.width, ButtonDim.height, m_BackgroundColor);
    }

    DrawText(m_Text.c_str(), ButtonDim.x + TextPosition.x, ButtonDim.y + TextPosition.y, FontSize, m_TextColor);
}

std::shared_ptr<UIEvent> Button::OnClickEvent()
{
    std::shared_ptr<UIEvent> EventUI = std::make_shared<UIEvent>();
    EventUI->ClickedUIElement = this;
    EventUI->Payload = m_Payload;
    return EventUI;
}

bool Button::IsClicked(Vector2 MousePosition, bool IsHovered)
{
    return IsMouseButtonPressed(0) && IsHovered;
}