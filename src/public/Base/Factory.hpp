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
	SoftObjectPath<T> NewObject(Args&&... args)
	{
		if (!m_Outter) return nullptr;

		static_assert(std::is_base_of_v<IObject, T>,
			"T must inherit from IObject");

		auto Obj = std::shared_ptr<T>(
				new T(std::forward<Args>(args)...),
				[Outter = m_Outter](T* ptr) 
				{
					LOG_INFO(l_FACTORY, TEXT("Cleaned Up Object: '{}'", ptr->m_Name));
					Outter->UnregisterObject(ptr);
					GameInstance::GetInstance()->UnregisterAsset(ptr->m_Name);
					delete ptr;
				});
		
		LOG_INFO(l_FACTORY, TEXT("Created Object from Type '{}' with size: '{}' and Registred to Outter: '{}'", typeid(T).name(), sizeof(T), m_Outter->GetName()));


		std::shared_ptr<IObject> CastedObj = std::dynamic_pointer_cast<IObject>(Obj);
		std::string ClassName = CastedObj->GetStaticClass().name();
		std::replace(ClassName.begin(), ClassName.end(), ' ', '.');
		std::string GeneratedName = m_Outter->GetName() + "/" + ClassName;
		CastedObj->m_Name = GameInstance::GetInstance()->RegisterAsset(GeneratedName);
		m_Outter->RegisterObject(Obj);
		return SoftObjectPath<T>(CastedObj->GetName());
	}
};