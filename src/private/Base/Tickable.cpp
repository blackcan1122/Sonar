#include "Base/Tickable.h"

Tickable::~Tickable()
{
}

void Tickable::SetPosition(Vector2 NewPos)
{
	Position = NewPos;
}

void Tickable::SetColor(Color NewColor)
{
	FColor = NewColor;
}

void Tickable::SetUseGravity(bool Status)
{
	this->GravityAffects = Status;
}

void Tickable::SetIsBoundByScreen(bool Status)
{
	IsBoundByScreen = Status;
}


Vector2 Tickable::GetPosition()
{
	return Position;
}

Color Tickable::GetColor()
{
	return FColor;
}

Vector2 Tickable::CalculatePosition(Vector2& CurrentPos, Vector2& Velocity, float& Dampening, Vector2& Accel, const float Deltatime)
{
	return Vector2();
}
