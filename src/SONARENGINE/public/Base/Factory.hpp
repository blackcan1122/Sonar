#pragma once
#include "Base/Core.h"
#include "Base/EngineRuntimeContext.hpp"
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

		EngineRuntimeContext& RuntimeContext = m_Outter->GetRuntimeContext();
		auto OutterLifetimeGuard = m_Outter->GetLifetimeGuard();
		auto AssetRegistry = GetGlobalAssetRegistry();
		if (!AssetRegistry)
		{
			LOG_ERROR(l_FACTORY, SONAR_FMT("Global AssetRegistry was not initialized while creating '{}'", typeid(T).name()));
			return SoftObjectPath<T>();
		}

		// SharedPtr with custom Deleter
		auto Obj = std::shared_ptr<T>(
			new T(std::forward<Args>(args)...),
			// Custom Deleter to clean it from the Asset Registry and also from the GameMode
			[Outter = m_Outter, OutterLifetimeGuard, RuntimeContextPtr = &RuntimeContext](T *ptr)
			{
				LOG_INFO(l_FACTORY, SONAR_FMT("Cleaning Up Object: '{}'", ptr->m_Name));
				RuntimeContextPtr->GetKeyDispatcher().RemoveListener(ptr->m_Name, KeyEvent::StaticClass());
				RuntimeContextPtr->GetMouseDispatcher().RemoveListener(ptr->m_Name, MouseEvent::StaticClass());
				LOG_INFO(l_FACTORY, SONAR_FMT("Object:'{}' Unsubscribed from 'OnMouseEvent' and 'OnKeyEvent' ", ptr->m_Name));

				if (!OutterLifetimeGuard.expired())
				{
					Outter->UnregisterObject(ptr);
				}
				if (auto AssetRegistry = GetGlobalAssetRegistry())
				{
					AssetRegistry->UnregisterAsset(ptr->m_Name);
				}
				delete ptr;
			});

		std::shared_ptr<IObject> CastedObj = std::dynamic_pointer_cast<IObject>(Obj); // Casting it to the actual Type
		std::string ClassName = CastedObj->GetStaticClass()->ClassName;

		LOG_INFO(l_FACTORY, SONAR_FMT("Created Object from Type '{}' with size: '{}' and Registred to Outter: '{}'", ClassName, sizeof(T), m_Outter->GetName()));

		std::replace(ClassName.begin(), ClassName.end(), ' ', '.');

		std::string GeneralName = m_Outter->GetObjectNamespace() + "/" + ClassName;
		CastedObj->m_Name = AssetRegistry->RegisterAsset(GeneralName);
		CastedObj->m_Outter = m_Outter;
		AssetRegistry->RegisterObject(CastedObj->m_Name, CastedObj);

		std::weak_ptr<IObject> WeakObj = CastedObj;

		RuntimeContext.GetKeyDispatcher().AddListener(
			CastedObj->m_Name,
			KeyEvent::StaticClass(),
			[WeakObj](std::shared_ptr<IEvent> evt)
			{
				if (auto CastedObj = WeakObj.lock())
				{
					auto CastedKeyEvent = std::dynamic_pointer_cast<KeyEvent>(evt);
					CastedObj->OnKeyStroke(CastedKeyEvent->KeyPressed, CastedKeyEvent->MousePos);
				}
			});
		LOG_INFO(l_FACTORY, SONAR_FMT("Class: '{}' subscribed to OnKeyEvent", CastedObj->m_Name));

		RuntimeContext.GetMouseDispatcher().AddListener(
			CastedObj->m_Name,
			MouseEvent::StaticClass(),
			[WeakObj](std::shared_ptr<IEvent> evt)
			{
				if (auto CastedObj = WeakObj.lock())
				{
					auto CastedKeyEvent = std::dynamic_pointer_cast<MouseEvent>(evt);
					CastedObj->OnMouseButtonPressed(CastedKeyEvent->KeyPressed, CastedKeyEvent->MousePos);
				}
			});
		LOG_INFO(l_FACTORY, SONAR_FMT("Class: '{}' subscribed to OnMouseEvent", CastedObj->m_Name));
		CastedObj->Initialize();
		LOG_INFO(l_FACTORY, SONAR_FMT("Class: '{}' Called 'Initialize()'", CastedObj->m_Name));
		m_Outter->RegisterObject(std::dynamic_pointer_cast<IObject>(Obj));
		return SoftObjectPath<T>(CastedObj->GetName());
	}
};