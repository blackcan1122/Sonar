#include "Base/World.hpp"
#include "Base/Helper.hpp"
#include "omp.h"

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

std::vector<int> World::GetAmbientLevel() const
{
    return m_CurrentAmbientLevel;
}
