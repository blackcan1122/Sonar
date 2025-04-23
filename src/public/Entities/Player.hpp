#include "Base/Core.h"
#include "Base/Entity.hpp"
#include "Entities/BaseSubmarine.hpp"

class Waterfall;

class Player : public BaseSubmarine
{
	AUTOBODY(Player, BaseSubmarine)

public:
	
	virtual void Tick(float DeltaTime) override;

	std::weak_ptr<Waterfall> WaterfallDisplay;

};