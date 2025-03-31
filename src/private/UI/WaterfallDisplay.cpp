#include "UI/WaterfallDisplay.hpp"

Waterfall::Waterfall(int Width, int Height)
    :Display(Width, Height)
{
    CurrentPixelBuffer = std::make_shared<PixelBuffer>(Width, Height);
}

Waterfall::~Waterfall()
{
}

void Waterfall::Update()
{
    CurrentPixelBuffer->ShiftPixelDown();
    for (int i = 0; i < CurrentPixelBuffer->m_Width; i++)
    {
        PixelData MyPixel(0, GetRandomValue(0, 45), 0, 255);
        (*CurrentPixelBuffer)[i] = MyPixel;
    }
}

void Waterfall::Draw()
{
    BeginTextureMode(ActiveRenderTarget);
    ClearBackground(DARKBLUE);
    for (int y = 0; y < CurrentPixelBuffer->m_Width; y++)
    {
        for (int x = 0; x < CurrentPixelBuffer->m_Height; x++)
        {
            int index = y * CurrentPixelBuffer->m_Width + x;
            DrawPixel(x, y, (*CurrentPixelBuffer)[index]);
        }
    }
    // Custom map drawing
    EndTextureMode();
}
