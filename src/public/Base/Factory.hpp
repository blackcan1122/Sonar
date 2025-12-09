#pragma once
#include "Base/Core.h"
#include "Base/GameMode.h"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

/**
 * @class Factory
 * @brief Handles the creation and lifecycle management of objects within a game mode.
 *
 * Purpose:
 * - Provides a type-safe way to create and manage objects.
 * - Registers created objects with the owning `GameMode`.
 *
 * Usage:
 * - Use `NewObject<T>(args...)` to create objects.
 * - Objects are automatically registered and cleaned up via custom deleters.
 *
 * Notes:
 * - Ensure that the factory is properly associated with a `GameMode`.
 * - Use `SoftObjectPath` for referencing created objects.
 */
class Factory
{
public:
	Factory(GameMode *Outter)
		: m_Outter(Outter)
	{
	}

	GameMode *m_Outter = nullptr;

	template <typename T, typename... Args>
	SoftObjectPath<T> NewObject(Args &&...args)
	{
		static_assert(std::is_base_of_v<IObject, T>,
					  "T must inherit from IObject");

		// SharedPtr with custom Deleter
		auto Obj = std::shared_ptr<T>(
			new T(std::forward<Args>(args)...),
			// Custom Deleter to clean it from the Asset Registry and also from the GameMode
			[Outter = m_Outter](T *ptr)
			{
				LOG_INFO(l_FACTORY, TEXT("Cleaning Up Object: '{}'", ptr->m_Name));
				GameInstance::KeyDispatcher.RemoveListener(ptr->m_Name, KeyEvent::StaticClass());
				GameInstance::MouseDispatcher.RemoveListener(ptr->m_Name, MouseEvent::StaticClass());
				LOG_INFO(l_FACTORY, TEXT("Object:'{}' Unsubscribed from 'OnMouseEvent' and 'OnKeyEvent' ", ptr->m_Name));

				Outter->UnregisterObject(ptr);
				GameInstance::GetAssetRegistry()->UnregisterAsset(ptr->m_Name);
				delete ptr;
			});

		std::shared_ptr<IObject> CastedObj = std::dynamic_pointer_cast<IObject>(Obj); // Casting it to the actual Type
		std::string ClassName = CastedObj->GetStaticClass()->ClassName;

		LOG_INFO(l_FACTORY, TEXT("Created Object from Type '{}' with size: '{}' and Registred to Outter: '{}'", ClassName, sizeof(T), m_Outter->GetName()));

		std::replace(ClassName.begin(), ClassName.end(), ' ', '.');

		std::string GeneralName = m_Outter->GetName() + "/" + ClassName;
		CastedObj->m_Name = GameInstance::GetAssetRegistry()->RegisterAsset(GeneralName);

		std::weak_ptr<IObject> WeakObj = CastedObj; // Create weak_ptr BEFORE the lambdas

		GameInstance::KeyDispatcher.AddListener(
			CastedObj->m_Name,
			KeyEvent::StaticClass(),
			[WeakObj](std::shared_ptr<IEvent> evt)
			{
				if (auto CastedObj = WeakObj.lock()) // Try to get shared_ptr
				{
					auto CastedKeyEvent = std::dynamic_pointer_cast<KeyEvent>(evt);
					CastedObj->OnKeyStroke(CastedKeyEvent->KeyPressed, CastedKeyEvent->MousePos);
				}
			});
		LOG_INFO(l_FACTORY, TEXT("Class: '{}' subscribed to OnKeyEvent", CastedObj->m_Name));

		GameInstance::MouseDispatcher.AddListener(
			CastedObj->m_Name,
			MouseEvent::StaticClass(),
			[WeakObj](std::shared_ptr<IEvent> evt) // Capture weak_ptr instead
			{
				if (auto CastedObj = WeakObj.lock()) // Try to get shared_ptr
				{
					auto CastedKeyEvent = std::dynamic_pointer_cast<MouseEvent>(evt);
					CastedObj->OnMouseButtonPressed(CastedKeyEvent->KeyPressed, CastedKeyEvent->MousePos);
				}
			});
		LOG_INFO(l_FACTORY, TEXT("Class: '{}' subscribed to OnMouseEvent", CastedObj->m_Name));

		m_Outter->RegisterObject(Obj);

		// we just return a SoftObjectPath
		return SoftObjectPath<T>(CastedObj->GetName());
	}
};