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

class IEvent;

class EventDispatcher 
{
public:
	using EventCallback = std::function<void(std::shared_ptr<IEvent>)>;

	void AddListener(const std::string& Identifier, SClass* EventClass, EventCallback Callback);

	template<typename T>
	void AddListener(const std::string& Identifier, SClass* EventClass, T* Object, void (T::* MemberFunc)(std::shared_ptr<IEvent>))
	{
		AddListener(Identifier, EventClass, [Object, MemberFunc](std::shared_ptr<IEvent> Event) {
			(Object->*MemberFunc)(Event);
			});
	}

	bool RemoveListener(const std::string& Identifier, SClass* EventClass);

	// General Purpose Dispatcher or Specific Identifier Dispatcher
	void Dispatch(std::shared_ptr<IEvent> EventToDispatch, const std::string& Identifier = "", bool bUseIdentifier = false);

	int AmountOfListener(std::shared_ptr<IEvent> EventToDispatch);
	int AmountOfListener(SClass* ClassID);

	void DumpListeners() const;

	std::string m_Name;

private:

	// EventType to {Identifier to Callback}
	std::unordered_map<SClass*, std::unordered_map<std::string, EventCallback>> m_Listener;

	
};