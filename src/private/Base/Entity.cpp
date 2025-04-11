#include "Base/Entity.hpp"

void Entity::ConvertAngleToVector()
{
	// Rotate around 90, to accomodate for math radians
	float RadianAngle = (m_Rotation + 90.f) * (PI / 180.f);

	m_FacingVector.x = -cos(RadianAngle);
	m_FacingVector.y = -sin(RadianAngle);

	m_FacingVector = Vector2Normalize(m_FacingVector);
}
