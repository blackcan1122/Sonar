#include "UI/WaterfallDisplay.hpp"
#include "raylib.h"

Waterfall::Waterfall(int Width, int Height, int TimeFrame)
    : Display(Width, Height), WorkerDone(true), TimeFrameInSec(TimeFrame)
{
    // Initialize buffers
    FrontBuffer = std::make_shared<PixelBuffer>(Width, Height);
    BackBuffer = std::make_shared<PixelBuffer>(Width, Height);

    // Create textures with initial data
    Image frontImg = GenImageColor(Width, Height, BLACK);
    FrontTexture = LoadTextureFromImage(frontImg);
    UnloadImage(frontImg);
    SetTargetFPS(60);

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

    if (WorkerDone == false)
    {
        return;
    }

    {
        std::lock_guard<std::mutex> Lock(AccDeltaMutex);
        std::cout << AccDelta << std::endl;
        AccDelta += DeltaTime;
    }

    // Calculate lines to process
    float TimeStep = TimestepPerPixel();
    int LinesToShift = 0;
    {
        std::lock_guard<std::mutex> Lock(AccDeltaMutex);
        LinesToShift = static_cast<int>(AccDelta / TimeStep);
        std::cout << LinesToShift << std::endl;
        if (LinesToShift > 0)
        {
            AccDelta -= LinesToShift * TimeStep;
        }
    }

    //if (LinesToShift > 0)
    //{
    //    ProcessBackBuffer(LinesToShift);

    //    UpdateTexture(FrontTexture, FrontBuffer->PixelArray.data());
    //    std::swap(FrontBuffer, BackBuffer);

    //}

    // Start worker if needed
    if (LinesToShift > 0 && WorkerDone)
    {
        WorkerDone = false;
        WorkerFuture = std::async(std::launch::async, [this, LinesToShift]()
            {
                ProcessBackBuffer(LinesToShift);
                WorkerDone = true;
            });
    }

    if (WorkerDone)
    {
        UpdateTexture(FrontTexture, FrontBuffer->PixelArray.data());
        std::swap(FrontBuffer, BackBuffer);
    }
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
                (*BackBuffer)[abs(i - 2)] = myPixel;
                (*BackBuffer)[abs(i - 1)] = myPixel;
                (*BackBuffer)[i] = myPixel;
                (*BackBuffer)[i + 1] = myPixel;
                (*BackBuffer)[i + 2] = myPixel;
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
    EndTextureMode();
}

float Waterfall::TimestepPerPixel()
{
    return (float)TimeFrameInSec / FrontBuffer->m_Height;
}
