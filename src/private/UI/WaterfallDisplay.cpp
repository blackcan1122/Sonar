#include "UI/WaterfallDisplay.hpp"
#include "raylib.h"
#include <chrono>
#include <string>

Waterfall::Waterfall(int Width, int Height, int TimeFrame)
    : Display(Width, Height), WorkerDone(true), RenderReady(false), TimeFrameInSec(TimeFrame)
{
    // Initialize buffers
    FrontBuffer = std::make_shared<PixelBuffer>(Width, Height);
    BackBuffer = std::make_shared<PixelBuffer>(Width, Height);

    // Create textures with initial data
    Image frontImg = GenImageColor(Width, Height, BLACK);
    FrontTexture = LoadTextureFromImage(frontImg);
    UnloadImage(frontImg);
}

Waterfall::~Waterfall() 
{
    // Wait for worker to finish before cleanup
    if (WorkerFuture.valid()) 
    {
        WorkerFuture.wait();
    }
    UnloadTexture(FrontTexture);
}

void Waterfall::Tick(float DeltaTime) 
{
    // Temp to change index of signal
    if (IsKeyDown(KEY_A))
    {
        Index--;
    }
    else if (IsKeyDown(KEY_D))
    {
        Index++;
    }

    AccDelta += DeltaTime;


    // Calculate lines to process
    float TimeStep = TimestepPerPixel();

    int LinesToShift = 0;
    {
        LinesToShift = static_cast<int>(AccDelta / TimeStep);
        if (LinesToShift > 0)
        {
            AccDelta -= LinesToShift * TimeStep;
        }
    }

    if (WorkerDone.load() && RenderReady.load())
    {
        std::swap(FrontBuffer, BackBuffer);
        UpdateTexture(FrontTexture, FrontBuffer->PixelArray.data());
        std::swap(FrontBuffer, BackBuffer);

        /*
        * the second swap is a Issue, but for now i just leave it.
        * we should do more like a partial copy of the frame before, so we keep history
        * something like:
        * 
        * 
        if (Work > 0 && ThreadDone.load())
        {
            WorkerFuture = std::asynch(std::launch::asynch, [this]()
                {
                    ShiftPixelDown();
                    CopyFirstPreviousRow();
                    ProcessBackBuffer(Work);
                    ThreadDone.store(true);
                    RenderReady.store(true);
               });
        }
        * 
        */

        RenderReady.store(false);
    }

    if (LinesToShift > 0 && WorkerDone.load())
    {
        WorkerDone.store(false);
        WorkerFuture = std::async(std::launch::async, [this, LinesToShift]()
            {
                ProcessBackBuffer(LinesToShift);
                WorkerDone.store(true);
                RenderReady.store(true);
            });
    }

    Draw();
    RenderToMainBuffer();
}

void Waterfall::ProcessBackBuffer(int LinesToShift) 
{
    for (int line = 0; line < LinesToShift; line++)
    {
        BackBuffer->ShiftPixelDown();
        for (int i = 0; i < BackBuffer->m_Width; i++)
        {
            if (i == Index)
            {
                // just some quick hack to make signal line wider
                PixelData myPixel(0, 80, 0, 255);
                (*BackBuffer)[i] = myPixel;
                continue;
            }

            PixelData myPixel(0, GetRandomValue(0, 45), 0, 255);
            (*BackBuffer)[i] = myPixel;
        }
    }
}

void Waterfall::Draw() 
{
    BeginTextureMode(ActiveRenderTarget);
    ClearBackground(BLACK);
    DrawTexture(FrontTexture, 0, 0, WHITE);
    GenerateBearings();
    EndTextureMode();
}

float Waterfall::TimestepPerPixel()
{
    return (float)TimeFrameInSec / FrontBuffer->m_Height;
}

void Waterfall::GenerateBearings()
{
    int FontSize = 10;

    int NumberOfLines = 8; // Total lines (minor + major)
    int MajorBearingStep = 90; // Labels at every 90 degrees

    for (int i = 0; i <= NumberOfLines; i++) 
    {
        // Calculate position for all lines
        int Pos = (FrontBuffer->m_Width / NumberOfLines) * i;

        // Calculate bearing for this line
        int bearing = ((static_cast<int>((static_cast<float>(i) / NumberOfLines) * 360 - 180)) + 360) % 360;
        
        if (bearing % MajorBearingStep == 0)
        {
            // Last One
            if (i == NumberOfLines)
            {
                int TextLength = MeasureText(std::to_string(bearing).c_str(), FontSize);
                DrawText(std::to_string(bearing).c_str(), Pos - TextLength, 20, FontSize, PURPLE);
            }
            // First One
            else if (i == 0)
            {
                DrawText(std::to_string(bearing).c_str(), Pos, 20, 10, PURPLE);
            }
            else
            {
                int TextLength = MeasureText(std::to_string(bearing).c_str(), FontSize);
                DrawText(std::to_string(bearing).c_str(), Pos - (TextLength / 2), 20, FontSize, PURPLE);
            }
            DrawLine(Pos, 0, Pos, 15, PURPLE); // Long line for labeled bearings
        }

        else 
        {
            // Draw shorter line for minor bearings
            DrawLine(Pos, 0, Pos, 5, PURPLE); // Short line for unlabeled bearings
        }
    }
}
