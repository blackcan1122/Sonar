#include "Base/World.hpp"
#include "Base/Helper.hpp"
#include "omp.h"

// Continents
#include "CountryMap/continent_outline.h"

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
        return std::move(buffer);
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
