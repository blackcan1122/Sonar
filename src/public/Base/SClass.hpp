#pragma once
#include <string>
#include <unordered_map>
#include <iostream>

class SClass
{
public:

	explicit SClass(const SClass* Parent, const char* Name)
		: ParentClass(Parent), ClassName(Name)
	{
		GetClassRegistry()[Name] = this;
	}

	bool operator<<(SClass* Other) const
	{
		const SClass* current = this;
		while (current) 
		{
			if (current == Other)
			{
				return true;
			}

			current = current->ParentClass;
		}
		return false;
	}

	static SClass* FindClass(const std::string& Name) {
		auto& Registry = GetClassRegistry();
		auto it = Registry.find(Name);
		return (it != Registry.end()) ? it->second : nullptr;
	}

	const std::string ClassName;

	static std::unordered_map<std::string, SClass*>& GetClassRegistry() {
		static std::unordered_map<std::string, SClass*> Registry;
		return Registry;
	}

protected:

	const SClass* ParentClass;

private:


};