#include "Base/Entity.hpp"

#include "Rendering/Graphics.hpp"

void Entity::Initialize()
{
}

void Entity::ConvertAngleToVector() 
{
	// Rotate around 90, to accomodate for math radians
	float RadianAngle = (m_Rotation + 90.f) * (Sonar::PI / 180.f);

	m_FacingVector.x = -cos(RadianAngle);
	m_FacingVector.y = -sin(RadianAngle);

	m_FacingVector = Sonar::Vector2Normalize(m_FacingVector);
}
