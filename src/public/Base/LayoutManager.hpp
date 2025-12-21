#include "Base/Core.h"
#include "Base/EventDispatcher.hpp"
#include "Base/SoftObject.hpp"
#include <any>

class LayoutManager : public IObject
{
	AUTOBODY(LayoutManager, IObject)

public:

	virtual bool RegisterDisplay(SoftObjectPath<IObject> inDisplay, std::any LayoutData = {}) = 0;
	virtual bool UnregisterDisplay(SoftObjectPath<IObject> inDisplay) = 0;

	virtual void UpdateLayout() = 0;

	virtual void OnWindowResize(int newWidth, int newHeight) = 0;
};