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
    // Asia
    RenderBufferArrayLine RBAsia;
    RBAsia.AddVertices(AsiaVertices);
    RBAsia.AddOffsets(AsiaOffsets);
    RBAsia.AddCounts(AsiaCounts);
    RBAsia.LoadBuffer();
    Continents.push_back(RBAsia);

    // South America
    RenderBufferArrayLine RBSouthAmerica;
    RBSouthAmerica.AddVertices(SouthAmericaVertices);
    RBSouthAmerica.AddOffsets(SouthAmericaOffsets);
    RBSouthAmerica.AddCounts(SouthAmericaCounts);
    RBSouthAmerica.LoadBuffer();
    Continents.push_back(RBSouthAmerica);

    // Africa
    RenderBufferArrayLine RBAfrica;
    RBAfrica.AddVertices(AfricaVertices);
    RBAfrica.AddOffsets(AfricaOffsets);
    RBAfrica.AddCounts(AfricaCounts);
    RBAfrica.LoadBuffer();
    Continents.push_back(RBAfrica);

    // Europe
    RenderBufferArrayLine RBEurope;
    RBEurope.AddVertices(EuropeVertices);
    RBEurope.AddOffsets(EuropeOffsets);
    RBEurope.AddCounts(EuropeCounts);
    RBEurope.LoadBuffer();
    Continents.push_back(RBEurope);

    // North America
    RenderBufferArrayLine RBNorthAmerica;
    RBNorthAmerica.AddVertices(NorthAmericaVertices);
    RBNorthAmerica.AddOffsets(NorthAmericaOffsets);
    RBNorthAmerica.AddCounts(NorthAmericaCounts);
    RBNorthAmerica.LoadBuffer();
    Continents.push_back(RBNorthAmerica);

    // Oceania
    RenderBufferArrayLine RBOceania;
    RBOceania.AddVertices(OceaniaVertices);
    RBOceania.AddOffsets(OceaniaOffsets);
    RBOceania.AddCounts(OceaniaCounts);
    RBOceania.LoadBuffer();
    Continents.push_back(RBOceania);

    // Antarctica
    RenderBufferArrayLine RBAntarctica;
    RBAntarctica.AddVertices(AntarcticaVertices);
    RBAntarctica.AddOffsets(AntarcticaOffsets);
    RBAntarctica.AddCounts(AntarcticaCounts);
    RBAntarctica.LoadBuffer();
    Continents.push_back(RBAntarctica);

    // Seven Seas
    RenderBufferArrayLine RBSevenseas;
    RBSevenseas.AddVertices(SevenseasVertices);
    RBSevenseas.AddOffsets(SevenseasOffsets);
    RBSevenseas.AddCounts(SevenseasCounts);
    RBSevenseas.LoadBuffer();
    Continents.push_back(RBSevenseas);
}

std::vector<int> World::GetAmbientLevel() const
{
    return m_CurrentAmbientLevel;
}
