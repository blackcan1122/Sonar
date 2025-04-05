#include "Base/DispatcherEvent.hpp"


EventDispatcher* DispatcherEvent::GetDispatcher()
{
	return CurrentDispatcher;
}
