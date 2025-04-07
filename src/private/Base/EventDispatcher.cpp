#include "Base/EventDispatcher.hpp"
#include "Base/Event.hpp"

void EventDispatcher::AddListener(const std::string& Identifier, std::type_index EventClass, EventCallback Callback)
{
	m_Listener[EventClass][Identifier] = Callback;
}

bool EventDispatcher::RemoveListener(const std::string& Identifier, std::type_index EventClass)
{
	auto ClassIt = m_Listener.find(EventClass);
	if (ClassIt == m_Listener.end()) return false;

	auto& ListenerToFunctionMap = ClassIt->second;
	auto FunctionEntryIt = ListenerToFunctionMap.find(Identifier);

	if (FunctionEntryIt == ListenerToFunctionMap.end()) return false;

	ListenerToFunctionMap.erase(FunctionEntryIt);

	if (ListenerToFunctionMap.empty())
	{
		m_Listener.erase(ClassIt);
	}

	return true;
}


void EventDispatcher::Dispatch(std::shared_ptr<IEvent> EventToDispatch, const std::string& Identifier, bool bUseIdentifier)
{
	// Getting the Static Class of the Event
	const auto& EventClass = EventToDispatch->GetStaticClass();
	
	if (m_Listener.find(EventClass) != m_Listener.end())
	{
		// Generall Dispatch, Dispatching All Event Subscriber from the Same Event Type
		if (bUseIdentifier == false || Identifier.empty())
		{
			for (const auto& listener : m_Listener[EventClass])
			{
				listener.second(EventToDispatch);
			}
		}
		// Specific Dispatch with Identifier
		else
		{
			m_Listener[EventClass][Identifier](EventToDispatch);
		}

	}
}

int EventDispatcher::AmountOfListener(std::shared_ptr<IEvent> EventToDispatch)
{
	const auto& EventClass = EventToDispatch->GetStaticClass();

	if (m_Listener.find(EventClass) == m_Listener.end())
	{
		return 0;
	}

	return m_Listener[EventClass].size();
}
