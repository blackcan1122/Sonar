#pragma once
#include "Base/Core.h"
#include <typeindex>

/**
* 
*/
class IEventData
{
public:
    
    
    virtual std::type_index GetStaticClass() = 0;

};


template<typename Derived>
class EventData : public IEventData
{
public:
    virtual std::type_index GetStaticClass() { return typeid(Derived); };

    static std::type_index StaticClass() { return typeid(Derived); };
    
};
