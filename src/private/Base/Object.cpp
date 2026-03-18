#include "Base/Object.hpp"
#include "Base/GameInstance.h"
#include "Base/GameMode.h"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Base/EventDispatcher.hpp"

//IObject::~IObject()
//{
//	for (const auto& entry : m_RegisteredCallbacks)
//	{
//		if (auto DispatcherPtr = entry.EventDispatcher.lock())
//		{
//			DispatcherPtr->RemoveListener(entry.Identifier, entry.EventClass);
//		}
//	}
//}
//
//// Copy constructor - copies basic properties but NOT callbacks
//IObject::IObject(const IObject& Other)
//	: m_Name(Other.m_Name)
//	, m_DisplayName(Other.m_DisplayName)
//	, bIsMarkedForDestruction(Other.bIsMarkedForDestruction)
//{
//}
//
//IObject& IObject::operator=(const IObject& Other)
//{
//	if (this != &Other)
//	{
//		// First, unregister all existing callbacks from this object
//		for (const auto& entry : m_RegisteredCallbacks)
//		{
//			if (auto DispatcherPtr = entry.EventDispatcher.lock())
//			{
//				DispatcherPtr->RemoveListener(entry.Identifier, entry.EventClass);
//			}
//		}
//		m_RegisteredCallbacks.clear();
//
//		m_Name = Other.m_Name;
//		m_DisplayName = Other.m_DisplayName;
//		bIsMarkedForDestruction = Other.bIsMarkedForDestruction;
//		
//	}
//	return *this;
//}
//
//IObject::IObject(IObject&& Other) noexcept
//	: m_Name(std::move(Other.m_Name))
//	, m_DisplayName(std::move(Other.m_DisplayName))
//	, m_RegisteredCallbacks(std::move(Other.m_RegisteredCallbacks))
//	, bIsMarkedForDestruction(Other.bIsMarkedForDestruction)
//{
//	// Clear the source's destruction flag to prevent double-unregistration
//	Other.bIsMarkedForDestruction = false;
//}
//
//// Move assignment operator
//IObject& IObject::operator=(IObject&& Other) noexcept
//{
//	if (this != &Other)
//	{
//		// First, unregister all existing callbacks from this object
//		for (const auto& entry : m_RegisteredCallbacks)
//		{
//			if (auto DispatcherPtr = entry.EventDispatcher.lock())
//			{
//				DispatcherPtr->RemoveListener(entry.Identifier, entry.EventClass);
//			}
//		}
//
//		// Move properties from source
//		m_Name = std::move(Other.m_Name);
//		m_DisplayName = std::move(Other.m_DisplayName);
//		m_RegisteredCallbacks = std::move(Other.m_RegisteredCallbacks);
//		bIsMarkedForDestruction = Other.bIsMarkedForDestruction;
//
//		// Clear source's destruction flag
//		Other.bIsMarkedForDestruction = false;
//	}
//	return *this;
//}

GameMode* IObject::GetOutter()
{
	// This just temporary, since in the current State only one GameMode can be Active
	// Will be replaced, when several gamemodes can run cocurrently
	// Right now the GameMode will always be the Outter for every IObject Derived Class
	return GameInstance::g_ActiveStateMachine.GetCurrentGameMode();
}

void IObject::MarkForDestruction()
{
	bIsMarkedForDestruction = true;
}

bool IObject::IsMarkedForDestruction()
{
	return bIsMarkedForDestruction;
}

//void IObject::AddCallbackToEventDispatcher(std::weak_ptr<EventDispatcher> Dispatcher, const std::string& Identifier, SClass* EventClass, EventCallback Callback)
//{
//	if (auto DispatcherPtr = Dispatcher.lock())
//	{
//		DispatcherPtr->AddListener(Identifier, EventClass, Callback);
//		CallbackEntry entry;
//		entry.EventDispatcher = Dispatcher;
//		entry.Identifier = Identifier;
//		entry.EventClass = EventClass;
//		this->m_RegisteredCallbacks.push_back(entry);
//	}
//}

BEGIN_REFLECTION(IObject)
REFLECT(m_DisplayName, std::string)
END_REFLECTION

BEGIN_REFLECTION(Object)
END_REFLECTION