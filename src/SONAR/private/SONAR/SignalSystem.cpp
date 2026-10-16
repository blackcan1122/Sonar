#include "SONAR/SignalSystem.hpp"

#include "Base/EngineRuntimeContext.hpp"
#include "Base/EventDispatcher.hpp"
#include "Base/GameMode.h"
#include "SONAR/SoundEvent.hpp"

SignalSystem::~SignalSystem()
{
	if (m_AllPurposeDispatcher != nullptr && !m_SoundEventListenerId.empty())
	{
		m_AllPurposeDispatcher->RemoveListener(m_SoundEventListenerId, SoundEvent::StaticClass());
	}
}

void SignalSystem::Initialize()
{
	m_AllPurposeDispatcher = &GetOutter()->GetRuntimeContext().GetAllPurposeDispatcher();
	m_SoundEventListenerId = "SignalSystem_" + std::to_string(reinterpret_cast<uintptr_t>(this));
	m_AllPurposeDispatcher->AddListener(m_SoundEventListenerId, SoundEvent::StaticClass(), this, &SignalSystem::OnSoundEvent);
}

void SignalSystem::Tick(float DeltaTime)
{
	m_Signals = std::move(m_PendingSignals);
	m_PendingSignals.clear();
	m_CurrentAmbientLevel = CreateAmbientNoise(360);
}

std::vector<int> SignalSystem::GetAmbientLevel() const
{
	return m_CurrentAmbientLevel;
}

std::vector<AcousticSignal> SignalSystem::GetSignals() const
{
	return m_Signals;
}

void SignalSystem::OnSoundEvent(std::shared_ptr<IEvent> Event)
{
	if (!Event || !(*(Event->GetStaticClass()) << SoundEvent::StaticClass()))
	{
		return;
	}

	std::shared_ptr<SoundEvent> CastedSoundEvent = std::dynamic_pointer_cast<SoundEvent>(Event);
	if (!CastedSoundEvent)
	{
		return;
	}

	AcousticSignal CurrentSignal;
	CurrentSignal.SenderPosition = CastedSoundEvent->SoundOrigin;
	CurrentSignal.Strength = CastedSoundEvent->SignalStrength;
	CurrentSignal.SenderEntity = CastedSoundEvent->Sender;
	m_PendingSignals.push_back(CurrentSignal);
}

std::vector<int> SignalSystem::CreateAmbientNoise(int NumberOfData)
{
	std::vector<int> ReturnArray;
	ReturnArray.resize(NumberOfData);
	for (int i = 0; i < NumberOfData; i++)
	{
		ReturnArray[i] = GetRandomValue(0, 40);
	}
	return ReturnArray;
}