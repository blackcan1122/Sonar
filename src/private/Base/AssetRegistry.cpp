#include "Base/AssetRegistry.hpp"
#include "Base/SoftObject.hpp"
#include "Base/GameInstance.h"
#include "Base/GameMode.h"


// std

#include <set>
#include <sstream>

std::shared_ptr<IObject> AssetRegistry::LoadAssetFromSoftObjectPath(SoftObjectPath<IObject> Path)
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

	std::shared_ptr<IObject> CastedOBJ = std::dynamic_pointer_cast<IObject>(MapIT->second);

	return CastedOBJ;
}

std::string AssetRegistry::RegisterAsset(const std::string name)
{
	std::string TimerName = "RegisterTimer " + name;
	Timer<std::chrono::microseconds> RegisterTimer(TimerName);

	std::string FutureName = GenerateNextAvaiableName(name);
	std::string base;
	int num;
	if (!ParseAssetName(FutureName, base, num))
	{
		LOG_ERROR(l_ASSET_REGISTRY, TEXT("Error Parsing AssetName: '{}'", FutureName));
		return std::string("");
	}

	if (num == -1)
	{
		// Base name without number (implicit 0)
		g_AssetRegistry[base].insert(0);
	}
	else
	{
		g_AssetRegistry[base].insert(num);
	}

	LOG_INFO(l_ASSET_REGISTRY, TEXT("'{}' Added to Asset Registry", FutureName));
	return FutureName;
}

bool AssetRegistry::UnregisterAsset(const std::string name)
{
	std::string TimerName = "Unregister Timer " + name;
	Timer<std::chrono::microseconds> RegisterTimer(TimerName);

	std::string base;
	int num;
	if (!ParseAssetName(name, base, num))
	{
		LOG_ERROR(l_ASSET_REGISTRY, TEXT("Problem with Unregistering Asset: '{}'", name));
		return false;
	}

	auto it = g_AssetRegistry.find(base);
	if (it == g_AssetRegistry.end())
	{
		LOG_ERROR(l_ASSET_REGISTRY, TEXT("Couldn't find Asset in AssetRegistry: '{}'", name));
		return false;
	}

	if (num == -1) num = 0; // Handle base name removal

	it->second.erase(num);
	if (it->second.empty()) {
		g_AssetRegistry.erase(it);
	}
	LOG_INFO(l_ASSET_REGISTRY, TEXT("Successfully removed '{}' from Asset Registry", name));
	return true;
}

// TODO: refactor this to generate unique names
std::string AssetRegistry::GenerateNextAvaiableName(const std::string base_name)
{
	const auto& numbers = g_AssetRegistry[base_name];
	if (numbers.empty()) {
		return base_name;
	}
	int expected = 0;
	auto it = numbers.lower_bound(expected);

	while (it != numbers.end() && *it == expected)
	{
		expected++;
		++it;
	}

	if (expected == 0)
	{
		return base_name; // Use base name if 0 is available
	}

	// Format with leading zero for 2-digit numbers
	std::ostringstream oss;
	oss << base_name << "_" << std::setw(2) << std::setfill('0') << expected;
	return oss.str();
}


bool AssetRegistry::ParseAssetName(const std::string& FullName, std::string& OutBaseName, int32_t& OutNumber)
{
	size_t last_underscore = FullName.find_last_of('_');
	if (last_underscore == FullName.npos)
	{
		OutBaseName = FullName;
		OutNumber = -1;
		return true;
	}

	std::string NumberPart = FullName.substr(last_underscore + 1);
	if (NumberPart.empty())
	{
		return false;
	}

	try
	{
		size_t Index;
		int Num = std::stoi(NumberPart, &Index);
		if (Index != NumberPart.size())
		{
			return false;
		}

		OutBaseName = FullName.substr(0, last_underscore);
		OutNumber = Num;
		return true;
	}
	catch (...)
	{
		return false;
	}
}