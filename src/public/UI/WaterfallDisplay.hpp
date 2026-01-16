#pragma once
#include "Base/Core.h"
#include "UI/Display.hpp"
#include <atomic>
#include <future>
#include "Base/SoftObject.hpp"


class Entity;
class Player;

struct PixelData
{
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
	uint8_t A = 255;

	PixelData() = default;
	PixelData(Color inColor)
	{
		R = inColor.r;
		G = inColor.g;
		B = inColor.b;
		A = inColor.a;
	}

	PixelData(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA)
	{
		R = inR;
		G = inG;
		B = inB;
		A = inA;
	}

	operator Color() const
	{
		 Color ReturnColor{ R,G,B,A };
		 return ReturnColor;
	}
};

struct PixelBuffer
{
	std::vector<PixelData> PixelArray;

	unsigned int m_Width = 0;
	unsigned int m_Height = 0;

	PixelData& operator[] (int Index)
	{
		return PixelArray[Index];
	}

	PixelBuffer() = default;
	
	PixelBuffer(unsigned int Width, unsigned int Height)
	{
		PixelArray.resize(Width * Height);
		m_Width = Width;
		m_Height = Height;
		std::fill(
			PixelArray.begin(),
			PixelArray.end(),
			PixelData{}
		);
	}

	void SetSize(unsigned int Width, unsigned int Height)
	{
		PixelArray.resize(Width * Height);
		m_Width = Width;
		m_Height = Height;
		std::fill(
			PixelArray.begin(),
			PixelArray.end(),
			PixelData{}
		);
	}

	bool bIsInitialized()
	{
		return PixelArray.size() > 0;
	}

	void ShiftPixelDown()
	{
		// Calculate total number of pixels excluding the top row.
		size_t TotalPixelsToMove = (m_Height - 1) * m_Width;

		// Since we use a 1D array for the pixel, we can just move the complete block of memory
		std::memmove(
			PixelArray.data() + m_Width,			// destination: starting at second row
			PixelArray.data(),						// source: starting at first row
			TotalPixelsToMove * sizeof(PixelData)	// Amount of Work
		);
	}
};

class Waterfall : public Display
{
	AUTOBODY(Waterfall, Display)
public:
	Waterfall(int Width, int Height, int TimeFrameInSec);
	virtual ~Waterfall() override;

	virtual void Tick(float Deltatime) override;
	virtual void Initialize() override;
	virtual void Draw() override;
	virtual void MarkForDestruction() override;
	virtual void ResizeDisplay(int NewWidth, int NewHeight) override;

	void AssignPlayer(SoftObjectPath<Player> inPlayer);

private:

	float AccDelta = 0;
	std::mutex AccDeltaMutex;
	std::shared_ptr<PixelBuffer> FrontBuffer;
	std::shared_ptr<PixelBuffer> BackBuffer;
	Texture2D FrontTexture;
	std::atomic<bool> WorkerDone;
	std::atomic<bool> RenderReady;
	std::future<void> WorkerFuture;
	std::vector<int> m_CurrentAmbientLevel;
	std::atomic<bool> InDestruction;
	bool FirstUpdate = true;

	int TimeFrameInSec;

	std::vector<int> m_AccumulatedSignals;
	int Counter = 0;

	void ProcessBackBuffer(int LinesToShift, std::vector<int> AccumulatedSamples, int AmountOfSamples);
	void GenerateBearings();
	void GenerateTimeScale();

	std::weak_ptr<Entity> Listener;

	float TimestepPerPixel();

	SoftObjectPath<Player> AssignedPlayer;
};