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
    CurrentAmbientLevel = CreateAmbientNoise(360);
}
