#pragma once
#include "Base/Core.h"
#include <typeindex>

/**
 * @class IEventData
 * @brief Base interface for event payload data.
 *
 * Purpose:
 * - Enables events to carry custom payloads of various types.
 * - Provides a type-safe mechanism for handling event-specific data.
 *
 * Usage:
 * - Inherit from `IEventData` to define custom event payloads.
 * - Implement the `GetStaticClass()` method for type identification.
 *
 * Notes:
 * - Use with event dispatchers to decouple systems and pass data.
 * - Ensure derived classes define meaningful payload structures.
 */

class IEventData
{
public:
    
    
    virtual std::type_index GetStaticClass() { return typeid(*this); };

};



DECLARE_CLASS(EventData, IEventData)

    
END_CLASS


