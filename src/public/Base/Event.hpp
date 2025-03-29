#pragma once
#include "Base/Core.h"
#include <typeindex>



class IEvent
{
public:

	virtual ~IEvent() = default;
	virtual std::string GetName() const = 0;
	virtual std::type_index GetStaticClass() const = 0;

};


template<typename Derived>
class Event : public IEvent
{
public:

	virtual std::string GetName() const = 0;
	virtual std::type_index GetStaticClass() const override { return typeid(Derived); };
	static std::type_index StaticClass() { return typeid(Derived); };
};