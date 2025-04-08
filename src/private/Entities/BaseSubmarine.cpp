#include "Entities/BaseSubmarine.hpp"


void BaseSubmarine::Tick(float DeltaTime)
{

}

void BaseSubmarine::Accel(float Amount)
{
	SetEntityLocation(Vector2Add(GetEntityLocation(), Vector2Scale(GetFacingVector(), Amount)));
}
