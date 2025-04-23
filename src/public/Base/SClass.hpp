#pragma once
#include <string>

class SClass
{
public:

	explicit SClass(const SClass* Parent, const char* Name)
		: ParentClass(Parent), ClassName(Name)
	{}

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

	const std::string ClassName;

protected:

	const SClass* ParentClass;
};