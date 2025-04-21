#pragma once
#include "Base/Core.h"
#include "Base/GameInstance.h"
#include "Base/Helper.hpp"

// std
#include <set>

class AssetRegistry
{
public:

	template <typename T>
	std::shared_ptr<T> LoadAssetFromSoftObjectPath(SoftObjectPath<T> Path)
	{
		std::string FullPath = Path.ToString();
		size_t Index = FullPath.find_first_of("/");

		std::string GameMode = FullPath.substr(0, Index);
		std::string Object = FullPath.substr(Index + 1);

		if (GameInstance::GetCurrentGameMode()->GetName() != GameMode)
		{
			return nullptr;
		}

		auto MapIT = GameInstance::GetCurrentGameMode()->m_Objects.find(FullPath);

		if (MapIT == GameInstance::GetCurrentGameMode()->m_Objects.end())
		{
			return nullptr;
		}

		std::shared_ptr<T> CastedOBJ = std::dynamic_pointer_cast<T>(MapIT->second);

		return CastedOBJ;
	}

	std::string RegisterAsset(const std::string name);

	bool UnregisterAsset(const std::string name);

	std::string GenerateNextAvaiableName(const std::string base_name);

	bool ParseAssetName(const std::string& FullName, std::string& OutBaseName, int32_t& OutNumber);


protected:
	std::unordered_map<std::string, std::set<int32_t>> g_AssetRegistry;

};