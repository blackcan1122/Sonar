#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"

class Factory
{
public:

	Factory(GameMode* Outter)
		:m_Outter(Outter)
	{}

	GameMode* m_Outter;

	template<typename T, typename... Args>
	std::shared_ptr<T> NewObject(Args&&... args)
	{
		if (!m_Outter) return nullptr;

		static_assert(std::is_base_of_v<IObject, T>,
			"T must inherit from IObject");

		auto Obj = std::shared_ptr<T>(
				new T(std::forward<Args>(args)...),
				[Outter = m_Outter](T* ptr) 
				{
					std::cout << "It Worked i guess?" << std::endl;
					Outter->UnregisterObject(ptr);
					delete ptr;
				});

		m_Outter->RegisterObject(Obj);
		return Obj;
	}
};