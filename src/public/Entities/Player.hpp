#include "Base/Core.h"
#include "Base/Entity.hpp"
#include "Entities/BaseSubmarine.hpp"

class Waterfall;

DECLARE_CLASS(Player, BaseSubmarine)

public:
	
	virtual void Tick(float DeltaTime) override;

	std::weak_ptr<Waterfall> WaterfallDisplay;

END_CLASS