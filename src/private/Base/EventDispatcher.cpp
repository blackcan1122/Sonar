#include "Base/EventDispatcher.hpp"
#include "Base/Event.hpp"

void EventDispatcher::AddListener(const std::string& Identifier, std::type_index EventClass, EventCallback Callback)
{
	m_Listener[EventClass][Identifier] = Callback;
}

void EventDispatcher::RemoveListener(const std::string& Identifier, std::type_index EventClass)
{
	auto ClassIt = m_Listener.find(EventClass);
	if (ClassIt == m_Listener.end()) return;

	auto& ListenerToFunctionMap = ClassIt->second;
	auto FunctionEntryIt = ListenerToFunctionMap.find(Identifier);

	if (FunctionEntryIt == ListenerToFunctionMap.end()) return;

	ListenerToFunctionMap.erase(FunctionEntryIt);

	if (ListenerToFunctionMap.empty())
	{
		m_Listener.erase(ClassIt);
	}
}


void EventDispatcher::Dispatch(std::shared_ptr<IEvent> EventToDispatch)
{
	// Holen uns den Name des Events
	const auto& EventClass = EventToDispatch->GetStaticClass();
	
	// we check if we can find a std::function which has subscriped
	if (m_Listener.find(EventClass) != m_Listener.end())
	{
		// Falls sich jemand für das Event Interessiert
		// Dann Loopen wir durch alle Funktionen die sich für das Event Interessieren
		for (const auto& listener : m_Listener[EventClass])
		{
			listener.second(EventToDispatch);
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
