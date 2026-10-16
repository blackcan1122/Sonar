#pragma once
#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include "Base/Macros.h"

template<typename T>
struct SoftObjectPath;

class IObject;

class SONARENGINE_API AssetRegistry
{
public:

	std::shared_ptr<IObject> LoadAssetFromSoftObjectPath(SoftObjectPath<IObject> Path);

	std::string RegisterAsset(const std::string name);
	void RegisterObject(const std::string& name, const std::shared_ptr<IObject>& Object);

	bool UnregisterAsset(const std::string name);

	std::string GenerateNextAvaiableName(const std::string base_name);

	bool ParseAssetName(const std::string& FullName, std::string& OutBaseName, int32_t& OutNumber);


protected:
	std::unordered_map<std::string, std::set<int32_t>> g_AssetRegistry;
	std::unordered_map<std::string, std::weak_ptr<IObject>> g_LiveObjects;

};