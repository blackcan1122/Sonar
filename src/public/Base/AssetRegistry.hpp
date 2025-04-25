#pragma once
#include "Base/Core.h"
#include "Base/Helper.hpp"

template<typename T>
struct SoftObjectPath;

// std
#include <set>

class AssetRegistry
{
public:

	std::shared_ptr<IObject> LoadAssetFromSoftObjectPath(SoftObjectPath<IObject> Path);

	std::string RegisterAsset(const std::string name);

	bool UnregisterAsset(const std::string name);

	std::string GenerateNextAvaiableName(const std::string base_name);

	bool ParseAssetName(const std::string& FullName, std::string& OutBaseName, int32_t& OutNumber);


protected:
	std::unordered_map<std::string, std::set<int32_t>> g_AssetRegistry;

};