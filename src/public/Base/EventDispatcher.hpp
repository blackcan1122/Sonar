#pragma once
#include "Base/Core.h"

class EventDispatcher 
{
public:
	using EventCallback = std::function<void(std::shared_ptr<IEvent>)>;

	void AddListener(const std::string& Identifier, std::type_index EventClass, EventCallback Callback);

	void RemoveListener(const std::string& Identifier, std::type_index EventClass);

	void Dispatch(std::shared_ptr<IEvent> EventToDispatch);

	int AmountOfListener(std::shared_ptr<IEvent> EventToDispatch);

	std::string Name;

private:
	std::unordered_map<std::type_index, std::unordered_map<std::string, EventCallback>> m_Listener;

	
};