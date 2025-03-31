#pragma once
#include "Base/Core.h"
#include "UI/Display.hpp"

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
		// Move all rows down by one (starting from bottom row)
		for (int y = m_Height - 1; y > 0; --y) {
			// Calculate row indices
			const size_t prev_row = (y - 1) * m_Width;
			const size_t curr_row = y * m_Width;

			// Copy previous row to current row
			std::copy(
				PixelArray.begin() + prev_row,
				PixelArray.begin() + prev_row + m_Width,
				PixelArray.begin() + curr_row
			);
		}
	}
};

DECLARE_CLASS(Waterfall, Display)

public:

	Waterfall(int Width, int Height);
	virtual ~Waterfall() override;

	virtual void Update() override;
	virtual void Draw() override;

	std::shared_ptr<PixelBuffer> CurrentPixelBuffer;


END_CLASS