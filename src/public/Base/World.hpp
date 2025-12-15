#pragma once
#include "Base/Core.h"
#include "Rendering/RenderBufferArrayLine.hpp"
#include "Base/NavalTypedefs.h"

// Need a Base Class and a interface with specific payloads
// For now we just keep it in this class

// For later we should Refactor this into a Soundprofile
struct Signals
{
	Vector2 SenderPosition;
	NavalUnits::Hz Strength;
};

enum class ETimeZone
{
	ZuluTime = 0, // GMT or UTC
	GMT_POS_01 = 1,
	GMT_POS_02 = 2,
	GMT_POS_03 = 3,
	GMT_POS_04 = 4,
	GMT_POS_05 = 5,
	GMT_POS_06 = 6,
	GMT_POS_07 = 7,
	GMT_POS_08 = 8,
	GMT_POS_09 = 9,
	GMT_POS_10 = 10,
	GMT_POS_11 = 11,
	GMT_POS_12 = 12,
	GMT_POS_13 = 13,
	GMT_POS_14 = 14,
	GMT_NEG_12 = -12,
	GMT_NEG_11 = -11,
	GMT_NEG_10 = -10,
	GMT_NEG_09 = -9,
	GMT_NEG_08 = -8,
	GMT_NEG_07 = -7,
	GMT_NEG_06 = -6,
	GMT_NEG_05 = -5,
	GMT_NEG_04 = -4,
	GMT_NEG_03 = -3,
	GMT_NEG_02 = -2,
	GMT_NEG_01 = -1
};

struct Time
{
	int Hour = 0;
	int Minutes = 0;
};

struct TimeOfDay
{
	Time LocalTime;
	Time ZuluTime;

	ETimeZone CurrentTimeZone = ETimeZone::ZuluTime;

	void SetTimeZone(ETimeZone NewTimezone)
	{
		CurrentTimeZone = NewTimezone;
	}

	void SetNewLocalTime(Time NewLocalTime)
	{
		LocalTime = NewLocalTime;
	}

	void SetNewZuliTime(Time NewZuluTime)
	{
		ZuluTime = NewZuluTime;
	}

	void ConvertLocalTimeToZulu()
	{
		ZuluTime.Hour = LocalTime.Hour + static_cast<int>(CurrentTimeZone);
	}
};

class World : public Object
{
	AUTOBODY(World, Object)

public:

	TimeOfDay m_TimeOfDay;
	int i = 0;

	virtual void Tick(float Deltatime) override;
	virtual void Initialize() override;

	std::vector<int> GetAmbientLevel() const;
	std::vector<Signals> GetSignals();
	std::vector<RenderBufferArrayLine> Continents;
	
	void ReceiveSound(std::shared_ptr<IEvent> Event);

protected:

	std::vector<int> CreateAmbientNoise(int NumberOfData);
	std::vector<int> m_CurrentAmbientLevel;

	std::vector<Signals> m_Signals;
	std::vector<Signals> m_PendingSignals;


};