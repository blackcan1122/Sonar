#pragma once
#include "Base/Core.h"

class SClass
{
public:

	explicit SClass(const SClass* Parent, const std::string Name)
		: ParentClass(Parent), ClassName(Name)
	{}

	bool operator<<(SClass* Other) const
	{
		const SClass* current = this;
		while (current) 
		{
			if (current == ParentClass)
			{
				return true;
			}

			current = current->ParentClass;
		}
		return false;
	}

protected:

	const SClass* ParentClass;
	const std::string ClassName;
};