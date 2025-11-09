#include "Base/Entity.hpp"

void Entity::ConvertAngleToVector()
{
	// Rotate around 90, to accomodate for math radians
	float RadianAngle = (m_Rotation + 90.f) * (PI / 180.f);

	m_FacingVector.x = -cos(RadianAngle);
	m_FacingVector.y = -sin(RadianAngle);

	m_FacingVector = Vector2Normalize(m_FacingVector);
}

void Entity::MoveEntityToPosition(Vector2 TargetPosition)
{
	Vector2 Delta = TargetPosition - m_Position;
	double AngleRadians = std::atan2(Delta.y, Delta.x);
	double AngleDeg = AngleRadians * (180.0f / PI);

	AngleDeg += 90.0f;
	AngleDeg = std::fmod(AngleDeg, 360.0f);
	if (AngleDeg < 0.0f)
	{
		AngleDeg += 360.0f;
	}

	this->m_DesiredCourse = ((int)AngleDeg % 360);
}