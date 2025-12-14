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

    InDestruction.store(false);
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
    // Handle resize interaction from base class
    Super::Tick(DeltaTime);
    
    auto CurrentWorld = GetOutter()->GetWorld().TryLoad();
    m_CurrentAmbientLevel.clear();
    if (CurrentWorld)
    {
        m_CurrentAmbientLevel = CurrentWorld->GetAmbientLevel();
    }

    // change to always accumulate data and process them, no matter how many lines we shift
    // but reset the sampled data when we shift

    if (InDestruction.load())
    {
        return;
    }

    // Temp to change index of signal
    if (IsKeyDown(KEY_A))
    {
        if (Index > 0) {
            Index--;
        }
    }
    else if (IsKeyDown(KEY_D))
    {
        if (Index < m_CurrentAmbientLevel.size() - 1) {
            Index++;
        }
    }

    // Scale ambient data to match buffer width
    int BufferWidth = static_cast<int>(FrontBuffer->m_Width);
    int SourceSize = static_cast<int>(m_CurrentAmbientLevel.size());
    
    // Resize accumulated signals to match buffer width (not source size)
    if (static_cast<int>(m_AccumulatedSignals.size()) != BufferWidth)
    {
        m_AccumulatedSignals.clear();
        m_AccumulatedSignals.resize(BufferWidth, 0);
    }
    
    // Interpolate/scale source data to buffer width
    if (SourceSize > 0)
    {
#pragma omp parallel for num_threads(4)
        for (int DestX = 0; DestX < BufferWidth; ++DestX)
        {
            // Map destination pixel to source sample using linear interpolation
            float SrcIndexF = (static_cast<float>(DestX) / BufferWidth) * SourceSize;
            int SrcIndex = static_cast<int>(SrcIndexF);
            float Frac = SrcIndexF - SrcIndex;
            
            // Clamp to valid range
            int SrcIndex0 = std::min(SrcIndex, SourceSize - 1);
            int SrcIndex1 = std::min(SrcIndex + 1, SourceSize - 1);
            
            // Linear interpolation between samples
            int Value = static_cast<int>(
                m_CurrentAmbientLevel[SrcIndex0] * (1.0f - Frac) + 
                m_CurrentAmbientLevel[SrcIndex1] * Frac
            );
            
            m_AccumulatedSignals[DestX] += Value;
        }
    }

    // Add bounds check for Index - scale index to buffer position
    // Since we're scaling from SourceSize to BufferWidth, one source sample
    // may span multiple destination pixels. Calculate the range of pixels affected.
    if (SourceSize > 0 && Index < static_cast<size_t>(SourceSize)) 
    {
        float PixelsPerSample = static_cast<float>(BufferWidth) / SourceSize;
        
        // Calculate start and end pixel for this source index
        int StartPixel = static_cast<int>(Index * PixelsPerSample);
        int EndPixel = static_cast<int>((Index + 1) * PixelsPerSample);
        
        // Clamp to valid range
        StartPixel = std::clamp(StartPixel, 0, BufferWidth - 1);
        EndPixel = std::clamp(EndPixel, StartPixel + 1, BufferWidth);
        
        // Fill all pixels that this source sample maps to
        for (int X = StartPixel; X < EndPixel; ++X)
        {
            m_AccumulatedSignals[X] += 255;
        }
    }
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

    int BufferWidth = static_cast<int>(BackBuffer->m_Width);
    
    for (int Line = 0; Line < LinesToShift; Line++)
    {
        BackBuffer->ShiftPixelDown();
        
        // Write scaled data to full buffer width
        size_t SignalSize = AverageSignals.empty() ? AccumulatedSamples.size() : AverageSignals.size();
        for (int X = 0; X < BufferWidth; ++X)
        {
            int Intensity = 0;
            if (X < SignalSize)
            {
                Intensity = AverageSignals.empty() ? AccumulatedSamples[X] : AverageSignals[X];
            }
            PixelData P(0, Intensity, 0, 255);
            (*BackBuffer)[X] = P;
        }
    }
}

void Waterfall::Draw() 
{
    BeginTextureMode(ActiveRenderTarget);
    ClearBackground(BLACK);
    DrawTexture(FrontTexture, 0, 0, WHITE);
    GenerateBearings();
    GenerateTimeScale();
    EndTextureMode();
}

void Waterfall::MarkForDestruction()
{
    InDestruction.store(true);

    if (WorkerFuture.valid())
    {
        WorkerFuture.wait();
    }

    LogInfo(l_HOUSE_KEEPING, TEXT("Cleaned Up Waterfall now Calling: {}", Super::GetStaticClass()->ClassName));
    Super::MarkForDestruction();
}

void Waterfall::ResizeDisplay(int NewWidth, int NewHeight)
{
    if (NewWidth <= 0 || NewHeight <= 0)
    {
        return;
    }
    
    // Don't resize if same size
    if (NewWidth == static_cast<int>(GetWidth()) && 
        NewHeight == static_cast<int>(GetHeight()))
    {
        return;
    }
    
    // Wait for any pending worker to finish before modifying buffers
    if (WorkerFuture.valid())
    {
        WorkerFuture.wait();
    }

    // Get the actual displayed data from the texture (this is what's really shown)
    Image oldImage = LoadImageFromTexture(FrontTexture);
    int oldWidth = oldImage.width;
    int oldHeight = oldImage.height;

    // Unload old texture
    UnloadTexture(FrontTexture);

    // Create new buffers with new dimensions
    FrontBuffer = std::make_shared<PixelBuffer>(NewWidth, NewHeight);
    BackBuffer = std::make_shared<PixelBuffer>(NewWidth, NewHeight);

    // Scale old image data into new buffer using bilinear interpolation
    for (int newY = 0; newY < NewHeight; ++newY) 
    {
        // Map new Y to old Y (scale vertically)
        float oldYf = (static_cast<float>(newY) / NewHeight) * oldHeight;
        int oldY0 = std::min(static_cast<int>(oldYf), oldHeight - 1);
        int oldY1 = std::min(oldY0 + 1, oldHeight - 1);
        float yFrac = oldYf - oldY0;
        
        for (int newX = 0; newX < NewWidth; ++newX) 
        {
            // Map new X to old X (scale horizontally)
            float oldXf = (static_cast<float>(newX) / NewWidth) * oldWidth;
            int oldX0 = std::min(static_cast<int>(oldXf), oldWidth - 1);
            int oldX1 = std::min(oldX0 + 1, oldWidth - 1);
            float xFrac = oldXf - oldX0;
            
            Color c00 = GetImageColor(oldImage, oldX0, oldY0);
            Color c10 = GetImageColor(oldImage, oldX1, oldY0);
            Color c01 = GetImageColor(oldImage, oldX0, oldY1);
            Color c11 = GetImageColor(oldImage, oldX1, oldY1);
            
            auto lerp = [](uint8_t a, uint8_t b, float t) -> uint8_t {
                return static_cast<uint8_t>(a * (1.0f - t) + b * t);
            };
            
            uint8_t r0 = lerp(c00.r, c10.r, xFrac);
            uint8_t r1 = lerp(c01.r, c11.r, xFrac);
            uint8_t g0 = lerp(c00.g, c10.g, xFrac);
            uint8_t g1 = lerp(c01.g, c11.g, xFrac);
            uint8_t b0 = lerp(c00.b, c10.b, xFrac);
            uint8_t b1 = lerp(c01.b, c11.b, xFrac);
            
            PixelData result;
            result.R = lerp(r0, r1, yFrac);
            result.G = lerp(g0, g1, yFrac);
            result.B = lerp(b0, b1, yFrac);
            result.A = 255;
            
            (*FrontBuffer)[newY * NewWidth + newX] = result;
        }
    }
    
    UnloadImage(oldImage);

    BackBuffer->PixelArray = FrontBuffer->PixelArray;
    
    Image frontImg = GenImageColor(NewWidth, NewHeight, BLACK);
    FrontTexture = LoadTextureFromImage(frontImg);
    UnloadImage(frontImg);
    
    // Upload scaled pixel data to texture
    UpdateTexture(FrontTexture, FrontBuffer->PixelArray.data());

    m_AccumulatedSignals.clear();
    m_AccumulatedSignals.resize(NewWidth, 0);
    Counter = 0;
    AccDelta = 0;
    
    WorkerDone.store(true);
    RenderReady.store(false);
    
    Super::ResizeDisplay(NewWidth, NewHeight);
    
    int Size = (FrontBuffer->m_Width * FrontBuffer->m_Height * sizeof(PixelData)) / 1024;
    LOG_INFO(l_RESOURCES, TEXT("Resized Waterfall to: {} x {} ({} KB)", 
        NewWidth, NewHeight, Size));
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

void Waterfall::GenerateTimeScale()
{
    int FontSize = 10;
    int BufferHeight = static_cast<int>(FrontBuffer->m_Height);
    int BufferWidth = static_cast<int>(FrontBuffer->m_Width);
    
    // Determine appropriate time step based on total time frame
    // We want roughly 4-8 major time markers
    float TimeStep;
    const char* UnitSuffix;
    
    if (TimeFrameInSec <= 10)
    {
        // For short timeframes (<=10s), show every 2 seconds
        TimeStep = 2.0f;
        UnitSuffix = "s";
    }
    else if (TimeFrameInSec <= 60)
    {
        // For medium timeframes (<=1min), show every 10 seconds
        TimeStep = 10.0f;
        UnitSuffix = "s";
    }
    else if (TimeFrameInSec <= 300)
    {
        // For longer timeframes (<=5min), show every 30 seconds
        TimeStep = 30.0f;
        UnitSuffix = "s";
    }
    else if (TimeFrameInSec <= 600)
    {
        // For timeframes up to 10min, show every minute
        TimeStep = 60.0f;
        UnitSuffix = "m";
    }
    else
    {
        // For very long timeframes, show every 2 minutes
        TimeStep = 120.0f;
        UnitSuffix = "m";
    }
    
    // Calculate how many markers we need
    int NumMarkers = static_cast<int>(TimeFrameInSec / TimeStep);
    
    // Draw time markers from top (now) to bottom (past)
    for (int i = 0; i <= NumMarkers; i++)
    {
        float TimeValue = i * TimeStep;
        float YPos = (TimeValue / TimeFrameInSec) * BufferHeight;
        
        // Skip if too close to top edge (where bearings are)
        if (YPos < 35) continue;
        
        // Format time label
        char TimeLabel[32];
        if (UnitSuffix[0] == 'm')
        {
            int Minutes = static_cast<int>(TimeValue / 60.0f);
            snprintf(TimeLabel, sizeof(TimeLabel), "-%d%s", Minutes, UnitSuffix);
        }
        else
        {
            snprintf(TimeLabel, sizeof(TimeLabel), "-%d%s", static_cast<int>(TimeValue), UnitSuffix);
        }
        
        // Draw horizontal tick line on left side
        bool IsMajor = (static_cast<int>(TimeValue) % static_cast<int>(TimeStep * 2) == 0) || i == 0;
        int TickLength = IsMajor ? 10 : 5;
        
        DrawLine(0, static_cast<int>(YPos), TickLength, static_cast<int>(YPos), ORANGE);
        
        // Draw label for major ticks
        if (IsMajor || NumMarkers <= 6)
        {
            DrawText(TimeLabel, TickLength + 2, static_cast<int>(YPos) - FontSize / 2, FontSize, ORANGE);
        }
        
        // Also draw tick on right side (optional mirror)
        DrawLine(BufferWidth - TickLength, static_cast<int>(YPos), BufferWidth, static_cast<int>(YPos), ORANGE);
    }
}
