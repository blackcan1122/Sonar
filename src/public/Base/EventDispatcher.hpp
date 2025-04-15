#pragma once
#include "Base/Core.h"
/**
* for Now we sort the Events like  
* EventClass to {Identifier to Callback}
* The reason for it, is we will have some Global dispatcher, and to speedup Lookup Times,
* we want minimize the minimize the workload here
* 
* TODO: Dispatch multithreaded
*/

class EventDispatcher 
{
public:
	using EventCallback = std::function<void(std::shared_ptr<IEvent>)>;

	void AddListener(const std::string& Identifier, std::type_index EventClass, EventCallback Callback);

	bool RemoveListener(const std::string& Identifier, std::type_index EventClass);

	// General Purpose Dispatcher or Specific Identifier Dispatcher
	void Dispatch(std::shared_ptr<IEvent> EventToDispatch, const std::string& Identifier = "", bool bUseIdentifier = false);

	int AmountOfListener(std::shared_ptr<IEvent> EventToDispatch);
	int AmountOfListener(std::type_index ClassID);

	std::string m_Name;

private:

	// EventType to {Identifier to Callback}
	std::unordered_map<std::type_index, std::unordered_map<std::string, EventCallback>> m_Listener;

	
};