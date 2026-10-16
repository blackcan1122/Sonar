#pragma once

#include "Base/Core.h"
#include "Base/NavalTypedefs.h"
#include "Base/SoftObject.hpp"

class Entity;
class IEvent;

struct AcousticSignal
{
	Vector2 SenderPosition;
	NavalUnits::Hz Strength;
	SoftObjectPath<Entity> SenderEntity;
};

class SignalSystem : public Object
{
	AUTOBODY(SignalSystem, Object)

public:
	SignalSystem() = default;
	virtual ~SignalSystem();

	virtual void Initialize() override;
	virtual void Tick(float DeltaTime) override;

	std::vector<int> GetAmbientLevel() const;
	std::vector<AcousticSignal> GetSignals() const;
	void OnSoundEvent(std::shared_ptr<IEvent> Event);

private:
	std::vector<int> CreateAmbientNoise(int NumberOfData);

	std::vector<int> m_CurrentAmbientLevel;
	std::vector<AcousticSignal> m_Signals;
	std::vector<AcousticSignal> m_PendingSignals;
	EventDispatcher* m_AllPurposeDispatcher = nullptr;
	std::string m_SoundEventListenerId;
};