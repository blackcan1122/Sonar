#include "Base/Core.h"
#include "Base/Entity.hpp"

class Waterfall;

DECLARE_CLASS(Player, Entity)

public:
	
	virtual void Tick(float DeltaTime) override;
	void Accel(float Amount);

	

	std::weak_ptr<Waterfall> WaterfallDisplay;

END_CLASS