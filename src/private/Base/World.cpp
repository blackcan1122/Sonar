#include "Base/World.hpp"
#include "Base/Helper.hpp"
#include "Base/Event.hpp"
#include "Events/SoundEvent.hpp"
#include "omp.h"

// Continents
#include "CountryMap/continent_outline.h"

void World::ReceiveSound(std::shared_ptr<IEvent> Event)
{
    if (*(Event->GetStaticClass()) << SoundEvent::StaticClass())
    {
        std::shared_ptr<SoundEvent> CastedSoundEvent = std::dynamic_pointer_cast<SoundEvent>(Event);
        Signals CurrentSignal;
        CurrentSignal.SenderPosition = CastedSoundEvent->SoundOrigin;
        CurrentSignal.Strength = CastedSoundEvent->SignalStrength;
        m_Signals.push_back(CurrentSignal);

    }
}

std::vector<Signals> World::GetSignals()
{
    // Return copy, don't clear - let Tick clean up at start of each frame
    return m_Signals;
}

std::vector<int> World::CreateAmbientNoise(int NumberOfData)
{
    std::vector<int> ReturnArray;
    ReturnArray.resize(NumberOfData);
    for (int i = 0; i < NumberOfData; i++)
    {
        ReturnArray[i] = GetRandomValue(0, 40);
    }
    return ReturnArray;
}

void World::Tick(float Deltatime)
{
    // Clear signals from last frame at the start of this frame
    // All consumers have had a chance to read them
    m_Signals.clear();
    
    m_CurrentAmbientLevel = CreateAmbientNoise(360);
}

void World::Initialize()
{
    auto CreateBuffer = [](const std::vector<float>& vertices,
                           const std::vector<unsigned int>& offsets,
                           const std::vector<unsigned int>& counts) noexcept -> RenderBufferArrayLine
    {
        RenderBufferArrayLine buffer;
        buffer.AddVertices(vertices);
        buffer.AddOffsets(offsets);
        buffer.AddCounts(counts);
        buffer.LoadBuffer();
        return buffer;
    };

    // Asia
    RenderBufferArrayLine RBAsia = CreateBuffer(AsiaVertices, AsiaOffsets, AsiaCounts);
    Continents.push_back(std::move(RBAsia));

    // South America
    RenderBufferArrayLine RBSouthAmerica = CreateBuffer(SouthAmericaVertices, SouthAmericaOffsets, SouthAmericaCounts);
    Continents.push_back(std::move(RBSouthAmerica));

    // Africa
    RenderBufferArrayLine RBAfrica = CreateBuffer(AfricaVertices, AfricaOffsets, AfricaCounts);
    Continents.push_back(std::move(RBAfrica));

    // Europe
    RenderBufferArrayLine RBEurope = CreateBuffer(EuropeVertices, EuropeOffsets, EuropeCounts);
    Continents.push_back(std::move(RBEurope));

    // North America
    RenderBufferArrayLine RBNorthAmerica = CreateBuffer(NorthAmericaVertices, NorthAmericaOffsets, NorthAmericaCounts);
    Continents.push_back(std::move(RBNorthAmerica));

    // Oceania
    RenderBufferArrayLine RBOceania = CreateBuffer(OceaniaVertices, OceaniaOffsets, OceaniaCounts);
    Continents.push_back(std::move(RBOceania));

    // Antarctica
    RenderBufferArrayLine RBAntarctica = CreateBuffer(AntarcticaVertices, AntarcticaOffsets, AntarcticaCounts);
    Continents.push_back(std::move(RBAntarctica));

    // Seven Seas
    RenderBufferArrayLine RBSevenseas = CreateBuffer(SevenseasVertices, SevenseasOffsets, SevenseasCounts);
    Continents.push_back(std::move(RBSevenseas));
}

std::vector<int> World::GetAmbientLevel() const
{
    return m_CurrentAmbientLevel;
}
