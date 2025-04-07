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
		if (!m_Outter) return nullptr; // Probably will never happen, as the Factory resided inside the GameMode

		static_assert(std::is_base_of_v<IObject, T>,
			"T must inherit from IObject");

		// SharedPtr with custom Deleter
		auto Obj = std::shared_ptr<T>(
				new T(std::forward<Args>(args)...),
				// Custom Deleter to clean it from the Asset Registry and also from the GameMode
				[Outter = m_Outter](T* ptr) 
				{
					LOG_INFO(l_FACTORY, TEXT("Cleaned Up Object: '{}'", ptr->m_Name));
					Outter->UnregisterObject(ptr);
					GameInstance::GetInstance()->UnregisterAsset(ptr->m_Name);
					delete ptr;
				});


		LOG_INFO(l_FACTORY, TEXT("Created Object from Type '{}' with size: '{}' and Registred to Outter: '{}'", typeid(T).name(), sizeof(T), m_Outter->GetName()));

		std::shared_ptr<IObject> CastedObj = std::dynamic_pointer_cast<IObject>(Obj); // Casting it to the actual Type
		std::string ClassName = CastedObj->GetStaticClass().name();

		// typeid.name returns a whitespace and we clean it and replaces it with a .
		std::replace(ClassName.begin(), ClassName.end(), ' ', '.');

		// Creating a Unique Name in the Asset Registry here
		std::string GeneralName = m_Outter->GetName() + "/" + ClassName;
		CastedObj->m_Name = GameInstance::GetInstance()->RegisterAsset(GeneralName);

		// Registering the created Obj to the GameMode
		m_Outter->RegisterObject(Obj);

		// we just return a SoftObjectPath
		return SoftObjectPath<T>(CastedObj->GetName());
	}
};