#include "UI/WaterfallDisplay.hpp"
#include "Base/GameMode.h"
#include "Base/World.hpp"
#include "raylib.h"
#include <chrono>
#include <string>
#include "omp.h"

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

    int Size = (FrontBuffer->m_Width * FrontBuffer->m_Height * sizeof(PixelData)) / 1024;
    LOG_INFO(l_RESOURCES, TEXT("Allocated Waterfall RenderTexture on VRAM with Size: {} x {} and {} Kilobytes", 
        FrontBuffer->m_Width, 
        FrontBuffer->m_Height, 
        Size));
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
    Timer<std::chrono::microseconds> t("Waterfall Tick");
    auto CurrentWorld = GetOutter()->GetWorld().TryLoad();
    m_CurrentAmbientLevel.clear();
    if (CurrentWorld)
    {
        m_CurrentAmbientLevel = CurrentWorld->GetAmbientLevel();
    }

    // change to always accumulate data and process them, no matter how many lines we shift
    // but reset the sampled data when we shift

    // Temp to change index of signal
    if (IsKeyDown(KEY_A))
    {
        Index--;
    }
    else if (IsKeyDown(KEY_D))
    {
        Index++;
    }

    if (m_AccumulatedSignals.empty())
    {
        m_AccumulatedSignals.resize(m_CurrentAmbientLevel.size(), 0);
    }

    #pragma omp parallel for
    for (int i = 0; i < m_CurrentAmbientLevel.size(); ++i)
    {
        m_AccumulatedSignals[i] += m_CurrentAmbientLevel[i];
    }

    m_AccumulatedSignals[Index] += 255;
    ++Counter;

    AccDelta += DeltaTime;

    // Calculate lines to process
    float TimeStep = TimestepPerPixel();

    int LinesToShift = static_cast<int>(AccDelta / TimeStep);

    if (WorkerDone.load() && RenderReady.load())
    {
        std::swap(FrontBuffer, BackBuffer);
        UpdateTexture(FrontTexture, FrontBuffer->PixelArray.data());
        std::swap(FrontBuffer, BackBuffer);
        RenderReady.store(false);


    }

    if (LinesToShift > 0 && WorkerDone.load())
    {
        WorkerDone.store(false);
        int temp = Counter;
        WorkerFuture = std::async(std::launch::async, [this, LinesToShift, temp]()
            {
                ProcessBackBuffer(LinesToShift, std::move(m_AccumulatedSignals), temp);
                WorkerDone.store(true);
                RenderReady.store(true);

            });

        Counter = 0;
        AccDelta -= LinesToShift * TimeStep;

    }

    Draw();
    RenderToMainBuffer();
}

void Waterfall::ProcessBackBuffer(int LinesToShift, std::vector<int> AccumulatedSamples, int AmountOfSamples) 
{
    std::vector<int> AverageSignals;
    if (AccumulatedSamples.size() > 0)
    {
        AverageSignals.resize(AccumulatedSamples.size());
        for (size_t i = 0; i < AccumulatedSamples.size(); ++i)
        {
            AverageSignals[i] = static_cast<int>(AccumulatedSamples[i] / (AmountOfSamples + 1));
        }
    }

    for (int line = 0; line < LinesToShift; line++)
    {
        BackBuffer->ShiftPixelDown();
        for (size_t x = 0; x < AverageSignals.size(); ++x)
        {
            int intensity = AverageSignals.empty() ? AccumulatedSamples[x] : AverageSignals[x];

                PixelData p(0, intensity, 0, 255);
                (*BackBuffer)[x] = p;
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

    int NumberOfLines = 8;
    int MajorBearingStep = 90;
    for (int i = 0; i <= NumberOfLines; i++) 
    {
        // Calculate position for all lines
        int Pos = (FrontBuffer->m_Width / NumberOfLines) * i;


        int bearing = ((static_cast<int>((static_cast<float>(i) / NumberOfLines) * 360 - 180)) + 360) % 360;
        
        if (bearing % MajorBearingStep == 0)
        {

            if (i == NumberOfLines)
            {
                int TextLength = MeasureText(std::to_string(bearing).c_str(), FontSize);
                DrawText(std::to_string(bearing).c_str(), Pos - TextLength, 20, FontSize, PURPLE);
            }

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
            DrawLine(Pos, 0, Pos, 5, PURPLE);
        }
    }
}
