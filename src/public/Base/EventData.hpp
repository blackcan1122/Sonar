#pragma once
#include "Base/Core.h"
#include <typeindex>

/**
* This class can be used to be Attached to a AllPurpose Event, so one Event Class can be used for different kind of Payloads
*/

class IEventData
{
public:
    
    
    virtual std::type_index GetStaticClass() { return typeid(*this); };

};



DECLARE_CLASS(EventData, IEventData)

    
END_CLASS


